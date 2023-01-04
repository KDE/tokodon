// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "maintimelinemodel.h"
#include "abstractaccount.h"
#include "timelinemodel.h"
#include <KLocalizedString>
#include <QUrlQuery>

MainTimelineModel::MainTimelineModel(QObject *parent)
    : TimelineModel(parent)
    , m_timelineName(QStringLiteral("home"))
{
    init();
}

QString MainTimelineModel::name() const
{
    return m_timelineName;
}

QString MainTimelineModel::displayName() const
{
    if (m_timelineName == "home") {
        if (m_manager && m_manager->rowCount() > 1) {
            if (m_manager->selectedAccount() == nullptr) {
                return i18n("Loading");
            }
            return i18nc("@title", "Home (%1)", m_manager->selectedAccount()->username());
        } else {
            return i18nc("@title", "Home");
        }
    } else if (m_timelineName == "public") {
        return i18nc("@title", "Local Timeline");
    } else if (m_timelineName == "federated") {
        return i18nc("@title", "Global Timeline");
    } else if (m_timelineName == "bookmarks") {
        return i18nc("@title", "Bookmarks");
    } else if (m_timelineName == "favourites") {
        return i18nc("@title", "Favourites");
    }
    return {};
}

void MainTimelineModel::setName(const QString &timelineName)
{
    if (timelineName == m_timelineName) {
        return;
    }

    m_timelineName = timelineName;
    Q_EMIT nameChanged();
    setLoading(false);
    fillTimeline();
}

void MainTimelineModel::fillTimeline(const QString &from_id)
{
    if (!m_account) {
        return;
    }

    if (m_timelineName != "home" && m_timelineName != "public" && m_timelineName != "federated" && m_timelineName != "bookmarks"
        && m_timelineName != "favourites") {
        return;
    }

    if (m_loading) {
        return;
    }
    setLoading(true);

    QString timelineName = m_timelineName;
    const bool local = timelineName == "public";

    // federated timeline is really "public" without local set
    if (timelineName == "federated") {
        timelineName = "public";
    }

    QUrlQuery q;
    if (local) {
        q.addQueryItem("local", "true");
    }
    if (!from_id.isEmpty()) {
        q.addQueryItem("max_id", from_id);
    }

    QString apiUrl;
    if (m_timelineName == "home" || m_timelineName == "public" || m_timelineName == "federated") {
        apiUrl = QStringLiteral("/api/v1/timelines/%1").arg(timelineName);
    } else if (m_timelineName == "bookmarks") {
        apiUrl = QStringLiteral("/api/v1/bookmarks");
    } else if (m_timelineName == "favourites") {
        apiUrl = QStringLiteral("/api/v1/favourites");
    }

    auto uri = m_account->apiUrl(apiUrl);
    uri.setQuery(q);

    auto account = m_account;
    auto currentTimelineName = m_timelineName;
    m_account->get(
        uri,
        true,
        this,
        [this, currentTimelineName, account, uri](QNetworkReply *reply) {
            if (m_account != account || m_timelineName != currentTimelineName) {
                setLoading(false);
                return;
            }

            fetchedTimeline(reply->readAll());
        },
        [this](QNetworkReply *reply) {
            Q_UNUSED(reply);
            setLoading(false);
        });
}

void MainTimelineModel::handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload)
{
    TimelineModel::handleEvent(eventType, payload);
    if (eventType == AbstractAccount::StreamingEventType::UpdateEvent && m_timelineName == "home") {
        const auto doc = QJsonDocument::fromJson(payload);
        const auto post = new Post(m_account, doc.object(), this);
        beginInsertRows({}, 0, 0);
        m_timeline.push_front(post);
        endInsertRows();
    }
}
