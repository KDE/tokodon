// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstracttimelinemodel.h"
#include "account.h"
#include "accountmodel.h"
#include "identity.h"
#include "poll.h"
#include "threadmodel.h"
#include <KLocalizedString>
#include <QtMath>

AbstractTimelineModel::AbstractTimelineModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

bool AbstractTimelineModel::loading() const
{
    return m_loading;
}

void AbstractTimelineModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
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
        {PollRole, QByteArrayLiteral("poll")},
        {VisibilityRole, QByteArrayLiteral("visibility")},
        {SelectedRole, QByteArrayLiteral("selected")},
    };
}

QVariant AbstractTimelineModel::postData(Post *post, int role) const
{
    switch (role) {
    case IdRole:
        return post->m_post_id;
    case MentionsRole:
        return post->mentions();
    case Qt::DisplayRole:
        return post->m_content;
    case AvatarRole:
        return post->authorIdentity()->avatarUrl();
    case AuthorIdRole:
        return post->authorIdentity()->account();
    case AuthorDisplayNameRole:
        return post->authorIdentity()->displayNameHtml();
    case WasRebloggedRole:
        return post->m_repeat;
    case RebloggedDisplayNameRole:
        if (post->repeatIdentity()) {
            return post->repeatIdentity()->displayNameHtml();
        }
        return {};
    case RebloggedIdRole:
        if (post->repeatIdentity()) {
            return post->repeatIdentity()->account();
        }
        return {};
    case PublishedAtRole:
        return post->m_published_at;
    case RebloggedRole:
        return post->m_isRepeated;
    case ReblogsCountRole:
        return post->m_repeatedCount;
    case FavoritedRole:
        return post->m_isFavorite;
    case FavoritesCountRole:
        return post->m_favoriteCount;
    case RepliesCountRole:
        return post->repliesCount();
    case PinnedRole:
        return post->m_pinned;
    case SensitiveRole:
        return post->m_isSensitive;
    case SpoilerTextRole:
        return post->subject();
    case VisibilityRole:
        return post->visibility();
    case AttachmentsRole:
        return QVariant::fromValue<QList<Attachment *>>(post->m_attachments);
    case ThreadModelRole:
        return QVariant::fromValue<QAbstractListModel *>(new ThreadModel(post->m_post_id));
    case CardRole:
        if (post->card().has_value()) {
            return QVariant::fromValue<Card>(*post->card());
        }
        return false;
    case AccountModelRole:
        return QVariant::fromValue<QAbstractListModel *>(new AccountModel(post->authorIdentity()->id(), post->authorIdentity()->account()));
    case RelativeTimeRole: {
        const auto current = QDateTime::currentDateTime();
        auto secsTo = post->m_published_at.secsTo(current);
        if (secsTo < 60 * 60) {
            const auto hours = post->m_published_at.time().hour();
            const auto minutes = post->m_published_at.time().minute();
            return i18nc("hour:minute",
                         "%1:%2",
                         hours < 10 ? QChar('0') + QString::number(hours) : QString::number(hours),
                         minutes < 10 ? QChar('0') + QString::number(minutes) : QString::number(minutes));
        } else if (secsTo < 60 * 60 * 24) {
            return i18n("%1h", qCeil(secsTo / (60 * 60)));
        } else if (secsTo < 60 * 60 * 24 * 7) {
            return i18n("%1d", qCeil(secsTo / (60 * 60 * 24)));
        }
        return QLocale::system().toString(post->m_published_at.date(), QLocale::ShortFormat);
    }
    case PollRole:
        if (post->poll()) {
            return QVariant::fromValue<Poll>(*post->poll());
        }
        return {};
    }
    return {};
}
