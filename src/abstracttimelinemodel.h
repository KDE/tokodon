// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "accountmanager.h"
#include "post.h"
#include <QAbstractListModel>
#include <qabstractitemmodel.h>

class Account;

class AbstractTimelineModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
public:
    enum CustoRoles {
        AvatarRole = Qt::UserRole + 1,
        IdRole,
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
        MentionsRole,
        RepliesCountRole,
        FavoritedRole,
        FavoritesCountRole,
        ThreadModelRole,
        AccountModelRole,
        UrlRole,
        CardRole,
        TypeRole,
        ExtraRole, ///< for sub-classes to extend the roles
    };

    AbstractTimelineModel(QObject *parent = nullptr);

    bool loading() const;

Q_SIGNALS:
    void loadingChanged();

protected:
    Account *m_account = nullptr;
    bool m_loading = true;
};
