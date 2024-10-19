// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "timeline/maintimelinemodel.h"

#include "texthandler.h"

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
        return i18nc("@title", "Home");
    } else if (m_timelineName == QStringLiteral("public")) {
        return i18nc("@title", "Local Timeline");
    } else if (m_timelineName == QStringLiteral("federated")) {
        return i18nc("@title", "Global Timeline");
    } else if (m_timelineName == QStringLiteral("bookmarks")) {
        return i18nc("@title", "Bookmarks");
    } else if (m_timelineName == QStringLiteral("favourites")) {
        return i18nc("@title", "Favorites");
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
    Q_EMIT listIdChanged();

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

void MainTimelineModel::fillTimeline(const QString &from_id, bool backwards)
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

    if (m_timelineName == QStringLiteral("home") && !fetchingLastId) {
        fetchLastReadId();
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
        // TODO: this is an *upper bound* so it always is one less than the last post we read
        // is this really how it's supposed to work wrt read markers?
        q.addQueryItem(QStringLiteral("max_id"), from_id);
    }

    QUrl uri;

    // FIXME: this logic is terrible and needs to be rewritten
    if (!backwards && !m_next) {
        // Fixes issues where on reaching the end the data is fetched from the start
        if (!m_next && !m_timeline.isEmpty()) {
            setLoading(false);
            return;
        }
        uri = !m_next ? m_account->apiUrl(
                            QStringLiteral("/api/v1/%1").arg(m_timelineName == QStringLiteral("trending") ? QStringLiteral("trends/statuses") : m_timelineName))
                      : m_next.value();
    } else if (backwards && !m_prev) {
        // Fixes issues where on reaching the end the data is fetched from the start
        if (!m_prev && !m_timeline.isEmpty()) {
            setLoading(false);
            return;
        }
        uri = !m_prev ? m_account->apiUrl(
                            QStringLiteral("/api/v1/%1").arg(m_timelineName == QStringLiteral("trending") ? QStringLiteral("trends/statuses") : m_timelineName))
                      : m_prev.value();
    } else if (publicTimelines.contains(m_timelineName)) {
        // federated timeline is really "public" without local set
        const QString apiUrl =
            QStringLiteral("/api/v1/timelines/%1").arg(m_timelineName == QStringLiteral("federated") ? QStringLiteral("public") : m_timelineName);
        uri = m_account->apiUrl(apiUrl);
        uri.setQuery(q);
    }

    if (m_timelineName == QStringLiteral("list")) {
        const QString apiUrl = QStringLiteral("/api/v1/timelines/list/%1").arg(m_listId);
        uri = m_account->apiUrl(apiUrl);
        uri.setQuery(q);
    } else if (publicTimelines.contains(m_timelineName) && !backwards) {
        // federated timeline is really "public" without local set
        const QString apiUrl =
            QStringLiteral("/api/v1/timelines/%1").arg(m_timelineName == QStringLiteral("federated") ? QStringLiteral("public") : m_timelineName);
        uri = m_account->apiUrl(apiUrl);
        uri.setQuery(q);
    } else if (!backwards) {
        // Fixes issues where on reaching the end the data is fetched from the start
        if (!m_next && !m_timeline.isEmpty()) {
            setLoading(false);
            return;
        }
        uri = !m_next ? m_account->apiUrl(
                            QStringLiteral("/api/v1/%1").arg(m_timelineName == QStringLiteral("trending") ? QStringLiteral("trends/statuses") : m_timelineName))
                      : m_next.value();
    }

    auto account = m_account;
    auto currentTimelineName = m_timelineName;
    m_account->get(
        uri,
        true,
        this,
        [this, currentTimelineName, account, backwards](QNetworkReply *reply) {
            if (m_account != account || m_timelineName != currentTimelineName) {
                setLoading(false);
                return;
            }

            const auto linkHeader = QString::fromUtf8(reply->rawHeader(QByteArrayLiteral("Link")));

            m_next = TextHandler::getNextLink(linkHeader);
            m_prev = TextHandler::getPrevLink(linkHeader);

            if (publicTimelines.contains(m_timelineName) && backwards) {
                int pos = fetchedTimeline(reply->readAll());
                Q_EMIT repositionAt(pos);
                setLoading(false);
            } else {
                fetchedTimeline(reply->readAll(), true);
                setLoading(false);
            }

            fetchedTimeline(reply->readAll(), !publicTimelines.contains(m_timelineName));
            setLoading(false);

            Q_EMIT hasPreviousChanged();

            // Only overwrite the read marker if they hit the button themselves
            if (m_userHasTakenReadAction && m_timelineName == QStringLiteral("home")) {
                // We want to force a refresh of the read marker in case we reached the top
                m_account->saveTimelinePosition(QStringLiteral("home"), m_timeline.first()->originalPostId());
            }
        },
        [this](QNetworkReply *reply) {
            Q_UNUSED(reply)
            setLoading(false);
        });
}

void MainTimelineModel::handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload)
{
    // Don't add streamed posts if we still have unread ones to go through
    if (!hasPrevious()) {
        TimelineModel::handleEvent(eventType, payload);
        if (eventType == AbstractAccount::StreamingEventType::UpdateEvent && m_timelineName == QStringLiteral("home")) {
            const auto doc = QJsonDocument::fromJson(payload);
            const auto post = new Post(m_account, doc.object(), this);
            beginInsertRows({}, 0, 0);
            m_timeline.push_front(post);
            endInsertRows();
        }
    }
}

bool MainTimelineModel::atEnd() const
{
    return !m_next;
}

void MainTimelineModel::reset()
{
    beginResetModel();
    qDeleteAll(m_timeline);
    m_timeline.clear();
    endResetModel();
    m_next = {};
    m_prev = {};
}

void MainTimelineModel::fetchLastReadId()
{
    if (fetchingLastId) {
        return;
    }

    fetchingLastId = true;

    QUrl uri = m_account->apiUrl(QStringLiteral("/api/v1/markers"));

    QUrlQuery urlQuery(uri);
    urlQuery.addQueryItem(QStringLiteral("timeline[]"), QStringLiteral("home"));
    uri.setQuery(urlQuery);

    m_account->get(uri, true, this, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());

        m_lastReadId = doc.object()[QLatin1String("home")].toObject()[QLatin1String("last_read_id")].toString();
        m_lastReadTime =
            QDateTime::fromString(doc.object()[QLatin1String("home")].toObject()[QLatin1String("updated_at")].toString(), Qt::ISODate).toLocalTime();

        Q_EMIT hasPreviousChanged();

        fetchedLastId = true;
        fillTimeline(m_lastReadId);
    });
}

void MainTimelineModel::fetchPrevious()
{
    m_userHasTakenReadAction = true;
    Q_EMIT userHasTakenReadActionChanged();
    fillTimeline({}, true);
}

bool MainTimelineModel::hasPrevious() const
{
    const bool lastReadTimeIsValid = m_lastReadTime.isValid();
    const bool hasPreviousLink = m_prev.has_value();
    const bool hasAnyPosts = !m_timeline.isEmpty();
    if (hasAnyPosts) {
        return lastReadTimeIsValid && hasPreviousLink;
    }
    return false;
}

QDateTime MainTimelineModel::lastReadTime() const
{
    return m_lastReadTime;
}

bool MainTimelineModel::userHasTakenReadAction() const
{
    return m_userHasTakenReadAction;
}

#include "moc_maintimelinemodel.cpp"
