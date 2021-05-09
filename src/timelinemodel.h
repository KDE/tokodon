// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "post.h"
#include "accountmanager.h"
#include <QAbstractListModel>

class TimelineModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(AccountManager *accountManager READ accountManager WRITE setAccountManager NOTIFY accountManagerChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY nameChanged)

public:
    enum CustoRoles {
        AvatarRole = Qt::UserRole +1,
        AuthorDisplayNameRole,
        PublishedAtRole,
        AuthorIdRole,
        TodayRole,
        SensitiveRole,
        SpoilerTextRole,
        MutedRole,
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
        UrlRole,
    };

    TimelineModel(QObject * parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    virtual void fillTimeline(QString fromId = QString());

    AccountManager *accountManager() const;
    void setAccountManager(AccountManager *accountManager);

    QString name() const;
    void setName(const QString &name);

    virtual QString displayName() const;

    void disallowUpdates() { m_last_fetch = time(nullptr) + 3; }
    std::shared_ptr<Post> internalData(const QModelIndex &index) const;


public Q_SLOTS:
    void actionReply(const QModelIndex &index);
    void actionFavorite(const QModelIndex &index);
    void actionRepeat(const QModelIndex &index);
    void actionMenu(const QModelIndex &index);
    void actionVis(const QModelIndex &index);

Q_SIGNALS:
    void wantReply(Account *account, std::shared_ptr<Post> post, const QModelIndex &index);
    void wantMenu(Account *account, std::shared_ptr<Post> post, const QModelIndex &index);
    void accountManagerChanged();
    void nameChanged();

protected:
    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;

    QString m_timeline_name;
    AccountManager *m_manager = nullptr;
    Account *m_account = nullptr;

    QList<std::shared_ptr<Post>> m_timeline;
    bool m_fetching;
    time_t m_last_fetch;

public Q_SLOTS:
    void fetchedTimeline(Account *account, QString original_name, QList<std::shared_ptr<Post>> posts);
};

