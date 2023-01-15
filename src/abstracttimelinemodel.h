// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "accountmanager.h"
#include "post.h"
#include <QAbstractListModel>
#include <qabstractitemmodel.h>

class AbstractAccount;

class AbstractTimelineModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
public:
    enum CustoRoles {
        AvatarRole = Qt::UserRole + 1,
        IdRole,
        AuthorDisplayNameRole,
        AuthorIdRole,
        AuthorUriRole,
        PublishedAtRole,
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
        BookmarkedRole,
        MentionsRole,
        RepliesCountRole,
        FavoritedRole,
        FavoritesCountRole,
        ThreadModelRole,
        UrlRole,
        CardRole,
        TypeRole,
        PollRole,
        VisibilityRole,
        SelectedRole,
        FiltersRole,
        ExtraRole, ///< for sub-classes to extend the roles
    };

    AbstractTimelineModel(QObject *parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    QVariant postData(Post *post, int role) const;

    bool loading() const;
    void setLoading(bool loading);

Q_SIGNALS:
    void loadingChanged();

protected:
    AbstractAccount *m_account = nullptr;
    bool m_loading = false;
};
