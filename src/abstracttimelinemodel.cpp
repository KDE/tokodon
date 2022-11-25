// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstracttimelinemodel.h"

AbstractTimelineModel::AbstractTimelineModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

bool AbstractTimelineModel::loading() const
{
    return m_loading;
}

QHash<int, QByteArray> AbstractTimelineModel::roleNames() const
{
    return {
        {Qt::DisplayRole, QByteArrayLiteral("display")},
        {AvatarRole, QByteArrayLiteral("avatar")},
        {AuthorDisplayNameRole, QByteArrayLiteral("authorDisplayName")},
        {PinnedRole, QByteArrayLiteral("pinned")},
        {IdRole, QByteArrayLiteral("id")},
        {AuthorIdRole, QByteArrayLiteral("authorId")},
        {PublishedAtRole, QByteArrayLiteral("publishedAt")},
        {MentionsRole, QByteArrayLiteral("mentions")},
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
        {AccountModelRole, QByteArrayLiteral("accountModel")},
        {CardRole, QByteArrayLiteral("card")},
        {TypeRole, QByteArrayLiteral("type")},
    };
}
