// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "notification/notificationmodel.h"

#include "account/abstractaccount.h"
#include "networkcontroller.h"
#include "texthandler.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>

NotificationModel::NotificationModel(QObject *parent)
    : AbstractTimelineModel(parent)
{
    m_manager = &AccountManager::instance();
    m_account = m_manager->selectedAccount();

    connect(m_manager, &AccountManager::accountSelected, this, [this](AbstractAccount *account) {
        if (m_account != account) {
            m_account = account;

            beginResetModel();
            m_notifications.clear();
            endResetModel();

            fillTimeline();
        }
    });

    connect(this, &NotificationModel::excludeTypesChanged, this, [this] {
        beginResetModel();
        m_notifications.clear();
        endResetModel();
        m_next = {};
        setLoading(false);
        fillTimeline();
    });

    setLoading(false);
    fillTimeline();
}

QStringList NotificationModel::excludeTypes() const
{
    return m_excludeTypes;
}

void NotificationModel::setExcludesTypes(const QStringList &excludeTypes)
{
    if (m_excludeTypes == excludeTypes) {
        return;
    }

    m_excludeTypes = excludeTypes;
    Q_EMIT excludeTypesChanged();
}

void NotificationModel::markAllNotificationsAsRead()
{
    // Doesn't make sense to mark anything as read if we aren't ready or yet
    if (m_notifications.isEmpty() || loading()) {
        return;
    }

    const QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = topLeft;
    for (int i = 0; i < rowCount({}); i++) {
        const auto index = AbstractTimelineModel::index(i, 0);
        if (data(index, UnreadRole).toBool()) {
            bottomRight = index;
        } else {
            break;
        }
    }

    m_lastReadId = QString::number(m_notifications.first()->id());
    Q_EMIT readMarkerChanged();

    // update all unread notifications so the section is reset in the UI
    Q_EMIT dataChanged(topLeft, bottomRight, {UnreadRole});

    m_account->saveTimelinePosition(QStringLiteral("notifications"), m_lastReadId);
    m_account->resetUnreadNotificationsCount();
}

bool NotificationModel::fullyRead() const
{
    return !data(index(0, 0), UnreadRole).toBool();
}

void NotificationModel::fillTimeline(const QUrl &next)
{
    if (!m_account) {
        return;
    }

    if (m_loading) {
        return;
    }

    if (!m_fetchedLastReadId) {
        fetchLastReadId();
        return;
    }

    setLoading(true);
    QUrl uri;
    if (next.isEmpty()) {
        uri = m_account->apiUrl(QStringLiteral("/api/v1/notifications"));
    } else {
        uri = next;
    }
    QUrlQuery urlQuery(uri);
    for (const auto &excludeType : std::as_const(m_excludeTypes)) {
        urlQuery.addQueryItem(QStringLiteral("exclude_types[]"), excludeType);
    }
    uri.setQuery(urlQuery);

    m_account->get(
        uri,
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto data = reply->readAll();
            const auto doc = QJsonDocument::fromJson(data);

            if (!doc.isArray()) {
                m_account->errorOccured(i18n("Error occurred when fetching the latest notification."));
                return;
            }

            const auto linkHeader = QString::fromUtf8(reply->rawHeader(QByteArrayLiteral("Link")));
            m_next = TextHandler::getNextLink(linkHeader);

            QList<std::shared_ptr<Notification>> notifications;
            const auto values = doc.array();
            for (const auto &value : values) {
                const QJsonObject obj = value.toObject();
                const auto notification = std::make_shared<Notification>(m_account, obj, this);

                notifications.push_back(notification);
            }

            if (notifications.isEmpty()) {
                setLoading(false);
                return;
            }

            beginInsertRows({}, m_notifications.count(), m_notifications.count() + notifications.count() - 1);
            m_notifications.append(notifications);
            endInsertRows();

            // This doesn't seem obvious, but we have to emit this so fullyRead() is marked as changed.
            // Otherwise you'll get a "Previous" heading on some pages.
            Q_EMIT readMarkerChanged();

            setLoading(false);
        },
        [this](QNetworkReply *reply) {
            setLoading(false);
            Q_EMIT NetworkController::instance().networkErrorOccurred(reply->errorString());
        });
}

void NotificationModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (m_notifications.isEmpty() || !m_next || m_loading) {
        return;
    }

    fillTimeline(m_next.value());
}

bool NotificationModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return !loading() && m_next.has_value();
}

int NotificationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_notifications.size();
}

QVariant NotificationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    int row = index.row();
    auto notification = m_notifications[row];
    auto post = notification->post();

    switch (role) {
    case TypeRole:
        return notification->type();
    case IsBoostedRole:
        return post != nullptr ? (post->boosted() || notification->type() == Notification::Repeat) : false;
    case BoostAuthorIdentityRole: {
        if (post != nullptr) {
            if (post->boostIdentity()) {
                return QVariant::fromValue<Identity *>(post->boostIdentity().get());
            }
            if (notification->type() == Notification::Repeat) {
                return QVariant::fromValue<Identity *>(notification->identity().get());
            }
        }
        return {};
    }
    case NotificationActorIdentityRole:
        return QVariant::fromValue(notification->identity().get());
    case AuthorIdentityRole: {
        if (notification->type() == Notification::Follow || notification->type() == Notification::FollowRequest) {
            return QVariant::fromValue<Identity *>(notification->identity().get());
        }
        if (post != nullptr) {
            return QVariant::fromValue<Identity *>(post->authorIdentity().get());
        }
    } break;
    case ReportRole:
        return QVariant::fromValue<ReportInfo *>(notification->report());
    case RelationshipSeveranceEventRole:
        return QVariant::fromValue<RelationshipSeveranceEvent>(*notification->relationshipSeveranceEvent());
    case AnnualReportEventRole:
        return QVariant::fromValue<AnnualReportEvent>(*notification->annualReportEvent());
    case ModerationWarningRole:
        return QVariant::fromValue<AccountWarning>(*notification->accountWarning());
    case UnreadRole:
        return notification->id() > m_lastReadId.toLongLong();
    case RelativeTimeRole:
        return TextHandler::getRelativeDateTime(notification->createdAt());
    default:
        if (post != nullptr) {
            return postData(post, role);
        }
    }

    return {};
}
QHash<int, QByteArray> NotificationModel::roleNames() const
{
    auto roles = AbstractTimelineModel::roleNames();
    roles.insert(ReportRole, QByteArrayLiteral("report"));
    roles.insert(RelationshipSeveranceEventRole, QByteArrayLiteral("relationshipSeveranceEvent"));
    roles.insert(ModerationWarningRole, QByteArrayLiteral("moderationWarning"));
    roles.insert(AnnualReportEventRole, QByteArrayLiteral("annualReportEvent"));
    roles.insert(UnreadRole, QByteArrayLiteral("unread"));
    return roles;
}

void NotificationModel::actionReply(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_notifications[row]->post();
    if (p != nullptr) {
        Q_EMIT wantReply(m_account, p, index);
    }
}

void NotificationModel::actionFavorite(const QModelIndex &index)
{
    int row = index.row();
    auto post = m_notifications[row]->post();
    if (post != nullptr) {
        AbstractTimelineModel::actionFavorite(index, post);
    }
}

void NotificationModel::actionRepeat(const QModelIndex &index)
{
    const int row = index.row();
    const auto post = m_notifications[row]->post();
    if (post != nullptr) {
        AbstractTimelineModel::actionRepeat(index, post);
    }
}

void NotificationModel::actionRedraft(const QModelIndex &index, bool isEdit)
{
    const int row = index.row();
    const auto p = m_notifications[row]->post();
    if (p != nullptr) {
        AbstractTimelineModel::actionRedraft(index, p, isEdit);
    }
}

void NotificationModel::actionBookmark(const QModelIndex &index)
{
    const int row = index.row();
    const auto post = m_notifications[row]->post();
    if (post != nullptr) {
        AbstractTimelineModel::actionBookmark(index, post);
    }
}

void NotificationModel::actionDelete(const QModelIndex &index)
{
    const auto p = m_notifications[index.row()]->post();
    if (p == nullptr) {
        return;
    }

    AbstractTimelineModel::actionDelete(index, p);

    // TODO: this sucks
    for (auto &notification : m_notifications) {
        if (notification->post() != nullptr && notification->post()->postId() == p->postId()) {
            int row = m_notifications.indexOf(notification);
            beginRemoveRows({}, row, row);
            m_notifications.removeOne(notification);
            endRemoveRows();
        }
    }
}

void NotificationModel::actionMute(const QModelIndex &index)
{
    const int row = index.row();
    const auto post = m_notifications[row]->post();
    if (post != nullptr) {
        AbstractTimelineModel::actionMute(index, post);
    }
}

void NotificationModel::fetchLastReadId()
{
    if (m_fetchingLastReadId) {
        return;
    }

    m_fetchingLastReadId = true;

    QUrl uri = m_account->apiUrl(QStringLiteral("/api/v1/markers"));

    QUrlQuery urlQuery(uri);
    urlQuery.addQueryItem(QStringLiteral("timeline[]"), QStringLiteral("notifications"));
    uri.setQuery(urlQuery);

    m_account->get(
        uri,
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());

            m_lastReadId = doc.object()[QLatin1String("notifications")].toObject()[QLatin1String("last_read_id")].toString();
            m_fetchedLastReadId = true;
            Q_EMIT readMarkerChanged();

            fillTimeline({});
        },
        [this](QNetworkReply *reply) {
            Q_UNUSED(reply);

            // If you failed, give up
            m_fetchedLastReadId = true;

            fillTimeline({});
        });
}

#include "moc_notificationmodel.cpp"
