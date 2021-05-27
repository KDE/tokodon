// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "notificationmodel.h"
#include "accountmodel.h"
#include "threadmodel.h"
#include <KLocalizedString>
#include <QtMath>

NotificationModel::NotificationModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_account = AccountManager::instance().selectedAccount();

    QObject::connect(&AccountManager::instance(), &AccountManager::accountSelected, this, [=] (Account *account) {
        if (m_account == account) {
            return;
        }
        m_account = account;

        beginResetModel();
        m_timeline.clear();
        endResetModel();

        fillTimeline();
    });
    QObject::connect(&AccountManager::instance(), &AccountManager::fetchedTimeline, this, &NotificationModel::fetchedTimeline);
    QObject::connect(&AccountManager::instance(), &AccountManager::invalidated, this, [=] (Account *account) {
        if (m_account == account) {
            qDebug() << "Invalidating account" << account;

            beginResetModel();
            m_timeline.clear();
            endResetModel();

            fillTimeline();
        }
    });
    connect(this, &NotificationModel::excludeTypesChanged, this, [this] {
        if (m_account == account) {
            qDebug() << "Invalidating account" << account;

            beginResetModel();
            m_timeline.clear();
            endResetModel();

            fillTimeline();
        }
    });

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

void NotificationModel::fillTimeline(const QString &fromId)
{
    m_fetching = true;

    if (m_account) {
        QUrl uri(m_account->m_instance_uri);
        uri.setPath(QStringLiteral("/api/v1/notifications"));
        if (!fromId.isEmpty()) {
            QUrlQuery q;
            q.addQueryItem(QStringLiteral("max_id"), fromId);
            uri.setQuery(q);
        }
        m_account->get(uri, true, [=] (QNetworkReply *reply) {
            const auto data = reply->readAll();
            const auto doc = QJsonDocument::fromJson(data);

            if (!doc.isArray()) {
                qDebug() << data;
                return;
            }

            QList<std::make_shared<Notification>> notifications;
            for (const auto &value : doc.array()) {
                QJsonObject obj = value.toObject();

                auto notification = std::make_shared<Notification>(this, obj);
                notifications.push_back(notification);
            }
        });
    }
}

void NotificationModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (m_notifications.size() < 1) {
        return;
    }

    auto notification = m_notifications.last();

    fillTimeline(notification->id());
}

bool NotificationModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return !m_fetching && time(nullptr) > m_last_fetch;
}

void NotificationModel::fetchedNotifications(QList<std::shared_ptr<Notification>> notifications)
{
    m_fetching = false;

    if (notifications.isEmpty())
        return;

    int row, last;

    if (!m_notifications.isEmpty()) {
        auto notification_old = m_notifications.first();
        auto notification_new = notifications.first();

        if (post_old->m_post_id > post_new->m_post_id) {
            row = m_timeline.size();
            last = row + posts.size() - 1;
            m_timeline += posts;
        } else {
            row = 0;
            last = posts.size();
            m_timeline = posts + m_timeline;
        }
    } else {
        row = 0;
        last = posts.size() - 1;
        m_timeline = posts;
    }

    beginInsertRows(QModelIndex(), row, last);
    endInsertRows();

    m_last_fetch = time(nullptr);
}

int NotificationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_timeline.size();
}

// this is even more extremely cursed
std::shared_ptr<Post> NotificationModel::internalData(const QModelIndex &index) const
{
    int row = index.row();
    return m_timeline[row];
}

QHash<int, QByteArray> NotificationModel::roleNames() const
{
    return {
        {Qt::DisplayRole, QByteArrayLiteral("display")},
        {AvatarRole, QByteArrayLiteral("avatar")},
        {AuthorDisplayNameRole, QByteArrayLiteral("authorDisplayName")},
        {PinnedRole, QByteArrayLiteral("pinned")},
        {AuthorIdRole, QByteArrayLiteral("authorId")},
        {PublishedAtRole, QByteArrayLiteral("publishedAt")},
        {RelativeTimeRole, QByteArrayLiteral("relativeTime")},
        {SensitiveRole, QByteArrayLiteral("sensitive")},
        {SpoilerTextRole, QByteArrayLiteral("spoilerText")},
        {RebloggedRole, QByteArrayLiteral("reblogged")},
        {WasRebloggedRole, QByteArrayLiteral("wasReblogged")},
        {RebloggedDisplayNameRole, QByteArrayLiteral("rebloggedDisplayName")},
        {RebloggedIdRole, QByteArrayLiteral("rebloggedId")},
        {AttachmentsRole, QByteArrayLiteral("attachments")},
        {ReblogsCountRole, QByteArrayLiteral("reblogsCount")},
        {RepliesCountRole, QByteArrayLiteral("repliesCount")},
        {FavoritedRole, QByteArrayLiteral("favorite")},
        {FavoritesCountRole, QByteArrayLiteral("favoritesCount")},
        {UrlRole, QByteArrayLiteral("url")},
        {ThreadModelRole, QByteArrayLiteral("threadModel")},
        {AccountModelRole, QByteArrayLiteral("accountModel")}
    };
}

QVariant NotificationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    int row = index.row();
    auto p = m_timeline[row];

    switch (role) {
    case Qt::DisplayRole:
        return p->m_content;
    case AvatarRole:
        return p->m_author_identity->m_avatarUrl;
    case AuthorDisplayNameRole:
        return p->m_author_identity->m_display_name;
    case AuthorIdRole:
        return p->m_author_identity->m_acct;
    case PublishedAtRole:
        return p->m_published_at;
    case WasRebloggedRole:
        return p->m_repeat;
    case RebloggedDisplayNameRole:
        if (p->m_repeat_identity) {
            return p->m_repeat_identity->m_display_name;
        }
        return {};
    case RebloggedIdRole:
        if (p->m_repeat_identity) {
            return p->m_repeat_identity->m_acct;
        }
        return {};
    case RebloggedRole:
        return p->m_isRepeated;
    case ReblogsCountRole:
        return p->m_repeatedCount;
    case FavoritedRole:
        return p->m_isFavorite;
    case PinnedRole:
        return p->m_pinned;
    case SensitiveRole:
        return p->m_isSensitive;
    case SpoilerTextRole:
        return p->m_subject;
    case AttachmentsRole:
        return QVariant::fromValue<QList<Attachment *>>(p->m_attachments);
    case ThreadModelRole:
        return QVariant::fromValue<QAbstractListModel *>(new ThreadModel(m_manager, p->m_post_id));
    case AccountModelRole:
        return QVariant::fromValue<QAbstractListModel *>(new AccountModel(m_manager, p->m_author_identity->m_id, p->m_author_identity->m_acct));
    case RelativeTimeRole:
        {
            const auto current = QDateTime::currentDateTime();
            auto secsTo = p->m_published_at.secsTo(current);
            if (secsTo < 60 * 60) {
                return i18nc("hour:minute", "%1:%2", p->m_published_at.time().hour(),
                        p->m_published_at.time().minute());
            } else if (secsTo < 60 * 60 * 24) {
                return i18n("%1h", qCeil(secsTo / (60 * 60)));
            } else if (secsTo < 60 * 60 * 24 * 7) { 
                return i18n("%1d", qCeil(secsTo / (60 * 60 * 24)));
            }
            return p->m_published_at.date().toString(Qt::SystemLocaleShortDate);
        }
    }

    return {};
}

void NotificationModel::actionReply(const QModelIndex &index)
{
    int row = index.row ();
    auto p = m_timeline[row];

    Q_EMIT wantReply (m_account, p, index);
}

void NotificationModel::actionMenu(const QModelIndex &index)
{
    int row = index.row ();
    auto p = m_timeline[row];

    Q_EMIT wantMenu (m_account, p, index);
}

void NotificationModel::actionFavorite(const QModelIndex &index)
{
    int row = index.row ();
    auto p = m_timeline[row];

    if (! p->m_isFavorite) {
        m_account->favorite(p);
        p->m_isFavorite = true;
    } else {
        m_account->unfavorite(p);
        p->m_isFavorite = false;
    }

    Q_EMIT dataChanged(index,index);
}

void NotificationModel::actionRepeat(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_timeline[row];

    if (!p->m_isRepeated) {
        m_account->repeat(p);
        p->m_isRepeated = true;
    } else {
        m_account->unrepeat(p);
        p->m_isRepeated = false;
    }

    Q_EMIT dataChanged(index, index);
}

void NotificationModel::actionVis(const QModelIndex &index)
{
    int row = index.row ();
    auto p = m_timeline[row];

    p->m_attachments_visible ^= true;

    Q_EMIT dataChanged(index, index);
}
