// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "linkpaginatedtimelinemodel.h"
#include "abstractaccount.h"
#include "timelinemodel.h"
#include <KLocalizedString>
#include <QUrlQuery>

LinkPaginationTimelineModel::LinkPaginationTimelineModel(QObject *parent)
    : TimelineModel(parent)
    , m_timelineName(QStringLiteral("favourites"))
{
    init();
}

QString LinkPaginationTimelineModel::name() const
{
    return m_timelineName;
}

QString LinkPaginationTimelineModel::displayName() const
{
    if (m_timelineName == "bookmarks") {
        return i18nc("@title", "Bookmarks");
    } else if (m_timelineName == "favourites") {
        return i18nc("@title", "Favourites");
    }
    return {};
}

void LinkPaginationTimelineModel::setName(const QString &timelineName)
{
    if (timelineName == m_timelineName) {
        return;
    }

    m_timelineName = timelineName;
    Q_EMIT nameChanged();
    setLoading(false);
    fillTimeline();
}

void LinkPaginationTimelineModel::fillTimeline(const QString &from_id)
{
    Q_UNUSED(from_id);

    if (!m_account) {
        return;
    }

    if (m_timelineName != "bookmarks" && m_timelineName != "favourites") {
        return;
    }

    if (m_loading) {
        return;
    }
    setLoading(true);

    QUrl uri;
    if (m_next.isEmpty()) {
        uri = m_account->apiUrl(QStringLiteral("/api/v1/%1").arg(m_timelineName));
    } else {
        uri = m_next;
    }

    auto account = m_account;
    auto currentTimelineName = m_timelineName;
    m_account->get(
        uri,
        true,
        this,
        [this, account, currentTimelineName](QNetworkReply *reply) {
            if (m_account != account || m_timelineName != currentTimelineName) {
                setLoading(false);
                return;
            }

            static QRegularExpression re("<(.*)>; rel=\"next\"");
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(next);
            m_next = QUrl::fromUserInput(match.captured(1));

            fetchedTimeline(reply->readAll());
        },
        [this](QNetworkReply *reply) {
            Q_UNUSED(reply);
            setLoading(false);
        });
}