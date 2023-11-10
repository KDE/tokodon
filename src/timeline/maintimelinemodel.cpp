// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "maintimelinemodel.h"

#include <KLocalizedString>

MainTimelineModel::MainTimelineModel(QObject *parent)
    : TimelineModel(parent)
{
    init();
}

QString MainTimelineModel::name() const
{
    return m_timelineName;
}

QString MainTimelineModel::displayName() const
{
    if (m_timelineName == QStringLiteral("home")) {
        if (m_manager && m_manager->rowCount() > 1) {
            if (m_manager->selectedAccount() == nullptr) {
                return i18n("Loading");
            }
            return i18nc("@title", "Home (%1)", m_manager->selectedAccount()->identity()->displayNameHtml());
        } else {
            return i18nc("@title", "Home");
        }
    } else if (m_timelineName == QStringLiteral("public")) {
        return i18nc("@title", "Local Timeline");
    } else if (m_timelineName == QStringLiteral("federated")) {
        return i18nc("@title", "Global Timeline");
    } else if (m_timelineName == QStringLiteral("bookmarks")) {
        return i18nc("@title", "Bookmarks");
    } else if (m_timelineName == QStringLiteral("favourites")) {
        return i18nc("@title", "Favourites");
    } else if (m_timelineName == QStringLiteral("trending")) {
        return i18nc("@title", "Trending");
    } else if (m_timelineName == QStringLiteral("list")) {
        return m_listId;
    }

    return {};
}

QString MainTimelineModel::listId() const
{
    return m_listId;
}

void MainTimelineModel::setListId(const QString &id)
{
    if (m_listId == id) {
        return;
    }

    m_listId = id;
    setLoading(false);
    fillTimeline({});
}

void MainTimelineModel::setName(const QString &timelineName)
{
    if (timelineName == m_timelineName) {
        return;
    }

    m_timelineName = timelineName;
    Q_EMIT nameChanged();
    setLoading(false);
    fillTimeline({});
}

void MainTimelineModel::fillTimeline(const QString &from_id)
{
    static const QSet<QString> validTimelines = {QStringLiteral("home"),
                                                 QStringLiteral("public"),
                                                 QStringLiteral("federated"),
                                                 QStringLiteral("bookmarks"),
                                                 QStringLiteral("favourites"),
                                                 QStringLiteral("trending"),
                                                 QStringLiteral("list")};
    static const QSet<QString> publicTimelines = {QStringLiteral("home"), QStringLiteral("public"), QStringLiteral("federated")};

    if (!m_account || m_loading || !validTimelines.contains(m_timelineName)) {
        return;
    }

    if (m_timelineName == QStringLiteral("list") && m_listId.isEmpty()) {
        return;
    }

    setLoading(true);

    const bool local = m_timelineName == QStringLiteral("public");

    QUrlQuery q;
    if (local) {
        q.addQueryItem(QStringLiteral("local"), QStringLiteral("true"));
    }
    if (!from_id.isEmpty()) {
        q.addQueryItem(QStringLiteral("max_id"), from_id);
    }

    QUrl uri;
    if (m_timelineName == QStringLiteral("list")) {
        const QString apiUrl = QStringLiteral("/api/v1/timelines/list/%1").arg(m_listId);
        uri = m_account->apiUrl(apiUrl);
        uri.setQuery(q);
    } else if (publicTimelines.contains(m_timelineName)) {
        // federated timeline is really "public" without local set
        const QString apiUrl =
            QStringLiteral("/api/v1/timelines/%1").arg(m_timelineName == QStringLiteral("federated") ? QStringLiteral("public") : m_timelineName);
        uri = m_account->apiUrl(apiUrl);
        uri.setQuery(q);
    } else {
        // Fixes issues where on reaching the end the data is fetched from the start
        if (m_next.isEmpty() && !m_timeline.isEmpty()) {
            setLoading(false);
            return;
        }
        uri = m_next.isEmpty() ? m_account->apiUrl(
                  QStringLiteral("/api/v1/%1").arg(m_timelineName == QStringLiteral("trending") ? QStringLiteral("trends/statuses") : m_timelineName))
                               : m_next;
    }

    auto account = m_account;
    auto currentTimelineName = m_timelineName;
    m_account->get(
        uri,
        true,
        this,
        [this, currentTimelineName, account](QNetworkReply *reply) {
            if (m_account != account || m_timelineName != currentTimelineName) {
                setLoading(false);
                return;
            }

            static QRegularExpression re(QStringLiteral("<(.*)>; rel=\"next\""));
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(QString::fromUtf8(next));
            m_next = QUrl::fromUserInput(match.captured(1));
            Q_EMIT atEndChanged();

            fetchedTimeline(reply->readAll(), !publicTimelines.contains(m_timelineName));
            setLoading(false);
        },
        [this](QNetworkReply *reply) {
            Q_UNUSED(reply)
            setLoading(false);
        });
}

void MainTimelineModel::handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload)
{
    TimelineModel::handleEvent(eventType, payload);
    if (eventType == AbstractAccount::StreamingEventType::UpdateEvent && m_timelineName == QStringLiteral("home")) {
        const auto doc = QJsonDocument::fromJson(payload);
        const auto post = new Post(m_account, doc.object(), this);
        beginInsertRows({}, 0, 0);
        m_timeline.push_front(post);
        endInsertRows();
    }
}

bool MainTimelineModel::atEnd() const
{
    return m_next.isEmpty();
}

void MainTimelineModel::reset()
{
    beginResetModel();
    qDeleteAll(m_timeline);
    m_timeline.clear();
    endResetModel();
    m_next.clear();
}

#include "moc_maintimelinemodel.cpp"
