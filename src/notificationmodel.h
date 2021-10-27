// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "accountmanager.h"
#include "post.h"
#include <QAbstractListModel>

class NotificationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QStringList excludeTypes READ excludeTypes WRITE setExcludesTypes NOTIFY excludeTypesChanged)

public:
    enum CustoRoles {
        AvatarRole = Qt::UserRole + 1,
        AuthorDisplayNameRole,
        PublishedAtRole,
        AuthorIdRole,
        RelativeTimeRole,
        SensitiveRole,
        SpoilerTextRole,
        MutedRole,
        PinnedRole,
        AttachmentsRole,
        WasRebloggedRole,
        RebloggedDisplayNameRole,
        RebloggedIdRole,
        RebloggedRole,
        ReblogsCountRole,
        RepliesCountRole,
        FavoritedRole,
        FavoritesCountRole,
        ThreadModelRole,
        AccountModelRole,
        UrlRole,
    };

    explicit NotificationModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    virtual void fillTimeline(const QString &fromId = QString());

    void disallowUpdates()
    {
        m_last_fetch = time(nullptr) + 3;
    }
    std::shared_ptr<Post> internalData(const QModelIndex &index) const;

public Q_SLOTS:
    void actionReply(const QModelIndex &index);
    void actionFavorite(const QModelIndex &index);
    void actionRepeat(const QModelIndex &index);
    void actionMenu(const QModelIndex &index);
    void actionVis(const QModelIndex &index);

Q_SIGNALS:
    void excludeTypesChanged();

protected:
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;

    QString m_timelineName;
    AccountManager *m_manager = nullptr;
    Account *m_account = nullptr;

    QList<std::shared_ptr<Notification>> m_notifications;
    bool m_fetching;
    time_t m_last_fetch;
    QStringList m_excludeTypes;

public Q_SLOTS:
    void fetchedNotifications(QList<std::shared_ptr<Notification>> notifications);
};
