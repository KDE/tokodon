// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "timeline/maintimelinemodel.h"

#include "networkcontroller.h"
#include "texthandler.h"

#include <KLocalizedString>
#include <config.h>

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
        return i18nc("@title", "Local");
    } else if (m_timelineName == QStringLiteral("federated")) {
        return i18nc("@title", "Global");
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

    fillTimeline({});
}

void MainTimelineModel::setName(const QString &timelineName)
{
    if (timelineName == m_timelineName) {
        return;
    }

    m_timelineName = timelineName;
    Q_EMIT nameChanged();
    fillTimeline({});
}

void MainTimelineModel::fillTimeline(const QString &fromId, bool backwards)
{
    static const QSet validTimelines = {QStringLiteral("home"),
                                        QStringLiteral("public"),
                                        QStringLiteral("federated"),
                                        QStringLiteral("bookmarks"),
                                        QStringLiteral("favourites"),
                                        QStringLiteral("trending"),
                                        QStringLiteral("list")};
    static const QSet publicTimelines = {QStringLiteral("home"), QStringLiteral("public"), QStringLiteral("federated")};

    const bool isHome = m_timelineName == QStringLiteral("home");
    const bool isList = m_timelineName == QStringLiteral("list");
    const bool isPublic = m_timelineName == QStringLiteral("public");
    const bool isTrending = m_timelineName == QStringLiteral("trending");
    const bool isFederated = m_timelineName == QStringLiteral("federated");

    // Ensure we aren't trying to load without an account, loading something else, or with an invalid timeline name.
    if (!m_account || loading() || !validTimelines.contains(m_timelineName)) {
        return;
    }

    // If we are fetching the home timeline, then make sure we fetch the read marker first before continuing.
    if (isHome && !fetchingLastId) {
        fetchLastReadId();
        return;
    }

    // If we are trying to load a list, don't continue without knowing which one to load.
    if (isList && m_listId.isEmpty()) {
        return;
    }

    setLoading(true);

    QUrl url;
    if (backwards) {
        // If we are moving backwards, use the prev url
        Q_ASSERT(m_prev.has_value());
        url = m_prev.value();
    } else {
        if (m_next) {
            // Otherwise, use the next url
            url = m_next.value();
        } else {
            // And if we're doing this for the first time, we need to know where to begin
            if (isTrending) {
                // Trending has a special URL
                url = m_account->apiUrl(QStringLiteral("/api/v1/trends/statuses"));
            } else if (isFederated) {
                // Federated timelines is "public" without local set
                url = m_account->apiUrl(QStringLiteral("/api/v1/timelines/public"));
            } else if (isList) {
                // List needs the list id appended to it
                url = m_account->apiUrl(QStringLiteral("/api/v1/timelines/list/%1").arg(m_listId));
            } else if (publicTimelines.contains(m_timelineName)) {
                url = m_account->apiUrl(QStringLiteral("/api/v1/timelines/%1").arg(m_timelineName));
            } else {
                url = m_account->apiUrl(QStringLiteral("/api/v1/%1").arg(m_timelineName));
            }
        }
    }

    auto query = QUrlQuery(url.query());
    if (isPublic) {
        query.addQueryItem(QStringLiteral("local"), QStringLiteral("true"));
    }
    if (!fromId.isEmpty() && !query.hasQueryItem(QStringLiteral("max_id"))) {
        // TODO: this is an *upper bound* so it always is one less than the last post we read
        // is this really how it's supposed to work wrt read markers?
        query.addQueryItem(QStringLiteral("max_id"), fromId);
    }
    url.setQuery(query);

    m_account->get(
        url,
        true,
        this,
        [this, currentTimelineName = m_timelineName, account = m_account, backwards](QNetworkReply *reply) {
            // This weird m_account != account is to protect against account switches that might happen while loading
            // Ditto for timeline name
            if (m_account != account || m_timelineName != currentTimelineName) {
                setLoading(false);
                return;
            }

            const auto linkHeader = QString::fromUtf8(reply->rawHeader(QByteArrayLiteral("Link")));

            m_next = TextHandler::getNextLink(linkHeader);
            Q_EMIT atEndChanged();

            m_prev = TextHandler::getPrevLink(linkHeader);

            if (publicTimelines.contains(m_timelineName) && backwards) {
                int const pos = fetchedTimeline(reply->readAll());
                Q_EMIT repositionAt(pos);
            } else {
                fetchedTimeline(reply->readAll(), true);
            }

            fetchedTimeline(reply->readAll(), !publicTimelines.contains(m_timelineName));

            // hasPrevious depends not just on m_prev, but also m_timeline!
            Q_EMIT hasPreviousChanged();

            setLoading(false);
        },
        [this](const QNetworkReply *reply) {
            setLoading(false);
            NetworkController::instance().networkErrorOccurred(reply->errorString());
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
            Q_EMIT streamedPostAdded(post->originalPostId());
        }
    }
}

bool MainTimelineModel::atEnd() const
{
    // Trending doesnt have pagination
    const bool isTrending = m_timelineName == QStringLiteral("trending");
    if (isTrending) {
        return true;
    }

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

bool MainTimelineModel::loading() const
{
    return m_timelineName.isEmpty() || TimelineModel::loading();
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

    m_account->get(
        uri,
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());

            m_lastReadId = doc.object()[QLatin1String("home")].toObject()[QLatin1String("last_read_id")].toString();
            if (m_initialLastReadId.isEmpty()) {
                m_initialLastReadId = m_lastReadId;
            }
            m_lastReadTime =
                QDateTime::fromString(doc.object()[QLatin1String("home")].toObject()[QLatin1String("updated_at")].toString(), Qt::ISODate).toLocalTime();

            Q_EMIT hasPreviousChanged();

            fetchedLastId = true;

            if (Config::continueReading()) {
                fillTimeline(m_lastReadId);
            } else {
                fillTimeline({});
            }
        },
        [=](QNetworkReply *reply) {
            Q_UNUSED(reply);

            // If you failed, give up
            fetchedLastId = true;

            if (Config::continueReading()) {
                fillTimeline(m_lastReadId);
            } else {
                fillTimeline({});
            }
        });
}

void MainTimelineModel::fetchPrevious()
{
    m_userHasTakenReadAction = true;
    Q_EMIT userHasTakenReadActionChanged();
    fillTimeline({}, true);
}

void MainTimelineModel::updateReadMarker(const QString &postId)
{
    const bool isHome = m_timelineName == QStringLiteral("home");

    // Only overwrite the read marker if they hit the button themselves
    if (isHome) {
        const auto lastReadId = m_lastReadId.toULongLong();
        if (postId.toULongLong() > lastReadId) {
            // We want to force a refresh of the read marker in case we reached the top
            m_account->saveTimelinePosition(QStringLiteral("home"), postId);
            m_lastReadId = postId;
        }
    }
}

bool MainTimelineModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return !atEnd();
}

QVariant MainTimelineModel::data(const QModelIndex &index, int role) const
{
    if (role != ShowReadMarkerRole) {
        return TimelineModel::data(index, role);
    }

    if (!fetchedLastId) {
        return false;
    }

    const auto postId = data(index, OriginalIdRole).toLongLong();
    return m_initialLastReadId.toLongLong() >= postId;
}

bool MainTimelineModel::hasPrevious() const
{
    if (!Config::continueReading()) {
        return false;
    }
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
