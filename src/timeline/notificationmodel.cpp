// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "notificationmodel.h"
#include "account/abstractaccount.h"
#include "abstracttimelinemodel.h"
#include <KLocalizedString>
#include <QUrlQuery>
#include <QtMath>

NotificationModel::NotificationModel(QObject *parent)
    : AbstractTimelineModel(parent)
{
    m_manager = &AccountManager::instance();
    m_account = m_manager->selectedAccount();

    connect(m_manager, &AccountManager::invalidated, this, [=](AbstractAccount *account) {
        if (m_account == account) {
            qDebug() << "Invalidating account" << account;

            beginResetModel();
            m_notifications.clear();
            endResetModel();
            m_next = QUrl();
            setLoading(false);
        }
    });

    connect(m_manager, &AccountManager::accountSelected, this, [=](AbstractAccount *account) {
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
        m_next = QUrl();
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

void NotificationModel::fillTimeline(const QUrl &next)
{
    if (!m_account) {
        return;
    }

    if (m_loading) {
        return;
    }
    setLoading(true);
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

        setLoading(false);

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

            if (notification.get()->post() != nullptr) {
                notifications.push_back(notification);
            }
        }

        if (notifications.isEmpty()) {
            return;
        }

        beginInsertRows({}, m_notifications.count(), m_notifications.count() + notifications.count() - 1);
        m_notifications.append(notifications);
        endInsertRows();
    });
}

void NotificationModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (m_notifications.isEmpty() || !m_next.isValid() || m_loading) {
        return;
    }

    fillTimeline(m_next);
}

bool NotificationModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    // Todo detect when there is nothing left
    return !loading();
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
        return post->boosted() || notification->type() == Notification::Repeat;
    case BoostAuthorIdentityRole:
        if (post->boostIdentity()) {
            return QVariant::fromValue<Identity *>(post->boostIdentity().get());
        }
        if (notification->type() == Notification::Repeat) {
            return QVariant::fromValue<Identity *>(notification->identity().get());
        }
        return {};
    case NotificationActorIdentityRole:
        return QVariant::fromValue(notification->identity().get());
    case AuthorIdentityRole:
        if (notification->type() == Notification::Follow || notification->type() == Notification::FollowRequest) {
            return QVariant::fromValue<Identity *>(notification->identity().get());
        } else {
            return QVariant::fromValue<Identity *>(post->authorIdentity().get());
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

void NotificationModel::actionFavorite(const QModelIndex &index)
{
    int row = index.row();
    auto post = m_notifications[row]->post();
    AbstractTimelineModel::actionFavorite(index, post);
}

void NotificationModel::actionRepeat(const QModelIndex &index)
{
    const int row = index.row();
    const auto post = m_notifications[row]->post();
    AbstractTimelineModel::actionRepeat(index, post);
}

void NotificationModel::actionRedraft(const QModelIndex &index, bool isEdit)
{
    const int row = index.row();
    const auto p = m_notifications[row]->post();

    AbstractTimelineModel::actionRedraft(index, p, isEdit);
}

void NotificationModel::actionBookmark(const QModelIndex &index)
{
    const int row = index.row();
    const auto post = m_notifications[row]->post();
    ;

    AbstractTimelineModel::actionBookmark(index, post);
}

void NotificationModel::actionDelete(const QModelIndex &index)
{
    const auto p = m_notifications[index.row()]->post();

    AbstractTimelineModel::actionDelete(index, p);

    // TODO: this sucks
    for (auto &notification : m_notifications) {
        if (notification->post()->postId() == p->postId()) {
            int row = m_notifications.indexOf(notification);
            beginRemoveRows({}, row, row);
            m_notifications.removeOne(notification);
            endRemoveRows();
        }
    }
}
