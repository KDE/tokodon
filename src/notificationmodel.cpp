// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "notificationmodel.h"
#include "abstractaccount.h"
#include "accountmodel.h"
#include "threadmodel.h"
#include <KLocalizedString>
#include <QUrlQuery>
#include <QtMath>

NotificationModel::NotificationModel(QObject *parent)
    : AbstractTimelineModel(parent)
{
    m_manager = &AccountManager::instance();
    m_account = m_manager->selectedAccount();

    QObject::connect(m_manager, &AccountManager::invalidated, this, [=](AbstractAccount *account) {
        if (m_account == account) {
            qDebug() << "Invalidating account" << account;

            beginResetModel();
            m_notifications.clear();
            endResetModel();
            m_next = QString();
            m_fetching = false;
        }
    });

    QObject::connect(m_manager, &AccountManager::accountSelected, this, [=](AbstractAccount *account) {
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
        m_next = QString();
        m_fetching = false;
        fillTimeline();
    });

    m_fetching = false;
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

void NotificationModel::fillTimeline(const QUrl &next)
{
    if (!m_account) {
        return;
    }

    if (m_fetching) {
        return;
    }
    m_fetching = true;
    QUrl uri;
    if (next.isEmpty()) {
        uri = QUrl::fromUserInput(m_account->instanceUri());
        uri.setPath(QStringLiteral("/api/v1/notifications"));
    } else {
        uri = next;
    }
    QUrlQuery urlQuery(uri);
    for (const auto &excludeType : std::as_const(m_excludeTypes)) {
        urlQuery.addQueryItem("exclude_types[]", excludeType);
    }
    uri.setQuery(urlQuery);

    m_account->get(uri, true, this, [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (m_loading) {
            m_loading = false;
            Q_EMIT loadingChanged();
        }

        if (!doc.isArray()) {
            m_account->errorOccured(i18n("Error occurred when fetching the latest notification."));
            return;
        }
        static QRegularExpression re("<(.*)>; rel=\"next\"");
        const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
        const auto match = re.match(next);
        m_next = QUrl::fromUserInput(match.captured(1));

        QList<std::shared_ptr<Notification>> notifications;
        const auto values = doc.array();
        for (const auto &value : values) {
            const QJsonObject obj = value.toObject();
            const auto notification = std::make_shared<Notification>(m_account, obj, this);
            notifications.push_back(notification);
        }
        fetchedNotifications(notifications);
    });
}

void NotificationModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (m_notifications.isEmpty() || !m_next.isValid() || m_fetching) {
        return;
    }

    fillTimeline(m_next);
}

bool NotificationModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    // Todo detect when there is nothing left
    return !m_fetching;
}

void NotificationModel::fetchedNotifications(QList<std::shared_ptr<Notification>> notifications)
{
    m_fetching = false;

    if (notifications.isEmpty()) {
        return;
    }

    beginInsertRows({}, m_notifications.count(), m_notifications.count() + notifications.count() - 1);
    m_notifications.append(notifications);
    endInsertRows();
}

int NotificationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_notifications.size();
}

// this is even more extremely cursed
std::shared_ptr<Notification> NotificationModel::internalData(const QModelIndex &index) const
{
    int row = index.row();
    return m_notifications[row];
}

QHash<int, QByteArray> NotificationModel::roleNames() const
{
    auto roles = AbstractTimelineModel::roleNames();
    roles.insert(NotificationActorIdentityRole, QByteArrayLiteral("notificationActorIdentity"));
    return roles;
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
    case WasRebloggedRole:
        return post->m_repeat || notification->type() == Notification::Repeat;
    case RebloggedDisplayNameRole:
        if (post->repeatIdentity()) {
            return post->repeatIdentity()->displayNameHtml();
        }
        if (notification->type() == Notification::Repeat) {
            return notification->identity()->displayName();
        }
        return {};
    case NotificationActorIdentityRole:
        return QVariant::fromValue(notification->identity().get());
    case AccountModelRole:
        if (notification->type() == Notification::Follow || notification->type() == Notification::FollowRequest) {
            return QVariant::fromValue<QAbstractListModel *>(new AccountModel(notification->identity()->id(), notification->identity()->account()));
        } else {
            return QVariant::fromValue<QAbstractListModel *>(new AccountModel(post->authorIdentity()->id(), post->authorIdentity()->account()));
        }
    default:
        return postData(post, role);
    }

    return {};
}

void NotificationModel::actionReply(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_notifications[row]->post();

    Q_EMIT wantReply(m_account, p, index);
}

void NotificationModel::actionMenu(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_notifications[row]->post();

    Q_EMIT wantMenu(m_account, p, index);
}

void NotificationModel::actionFavorite(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_notifications[row]->post();

    if (!p->m_isFavorite) {
        m_account->favorite(p);
        p->m_isFavorite = true;
    } else {
        m_account->unfavorite(p);
        p->m_isFavorite = false;
    }

    Q_EMIT dataChanged(index, index);
}

void NotificationModel::actionRepeat(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_notifications[row]->post();

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
    int row = index.row();
    auto p = m_notifications[row]->post();

    p->m_attachments_visible ^= true;

    Q_EMIT dataChanged(index, index);
}
