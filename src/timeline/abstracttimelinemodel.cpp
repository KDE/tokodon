// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "timeline/abstracttimelinemodel.h"

#include "account/abstractaccount.h"
#include "editor/attachmenteditormodel.h"
#include "editor/posteditorbackend.h"

using namespace Qt::Literals::StringLiterals;

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
        {IdRole, QByteArrayLiteral("id")},
        {OriginalIdRole, QByteArrayLiteral("originalId")},
        {UrlRole, QByteArrayLiteral("url")},
        {ContentRole, QByteArrayLiteral("content")},
        {SpoilerTextRole, QByteArrayLiteral("spoilerText")},
        {AuthorIdentityRole, QByteArrayLiteral("authorIdentity")},
        {PublishedAtRole, QByteArrayLiteral("publishedAt")},
        {VisibilityRole, QByteArrayLiteral("visibility")},
        {WasEditedRole, QByteArrayLiteral("wasEdited")},
        {EditedAtRole, QByteArrayLiteral("editedAt")},
        {SelectedRole, QByteArrayLiteral("selected")},
        {FiltersRole, QByteArrayLiteral("filters")},
        {RelativeTimeRole, QByteArrayLiteral("relativeTime")},
        {AbsoluteTimeRole, QByteArrayLiteral("absoluteTime")},
        {SensitiveRole, QByteArrayLiteral("sensitive")},
        {IsThreadReplyRole, "isThreadReply"},
        {IsLastThreadReplyRole, QByteArrayLiteral("isLastThreadReply")},

        // Additional content
        {CardRole, QByteArrayLiteral("card")},
        {ApplicationRole, QByteArrayLiteral("application")},
        {PollRole, QByteArrayLiteral("poll")},
        {MentionsRole, QByteArrayLiteral("mentions")},
        {AttachmentsRole, QByteArrayLiteral("attachments")},

        // Reblog
        {IsBoostedRole, "isBoosted"},
        {BoostAuthorIdentityRole, "boostAuthorIdentity"},

        // Reply
        {IsReplyRole, "isReply"},
        {ReplyAuthorIdentityRole, "replyAuthorIdentity"},

        // Interaction count
        {ReblogsCountRole, QByteArrayLiteral("reblogsCount")},
        {RepliesCountRole, QByteArrayLiteral("repliesCount")},
        {FavouritesCountRole, QByteArrayLiteral("favouritesCount")},

        // User self interaction
        {FavouritedRole, QByteArrayLiteral("favourited")},
        {RebloggedRole, QByteArrayLiteral("reblogged")},
        {MutedRole, QByteArrayLiteral("muted")},
        {BookmarkedRole, QByteArrayLiteral("bookmarked")},
        {PinnedRole, QByteArrayLiteral("pinned")},

        // Notification
        {NotificationActorIdentityRole, "notificationActorIdentity"},
        {TypeRole, "type"},
        {PostRole, "post"},
        {IsGroupRole, "isGroup"},
        {NumInGroupRole, "numInGroup"},
        {IsInGroupRole, "isInGroup"},
    };
}

QVariant AbstractTimelineModel::postData(Post *post, int role) const
{
    switch (role) {
    case IdRole:
        return post->postId();
    case OriginalIdRole:
        return post->originalPostId();
    case MentionsRole:
        return post->mentions();
    case ContentRole:
        return post->content();
    case AuthorIdentityRole:
        return QVariant::fromValue<Identity *>(post->authorIdentity().get());
    case IsBoostedRole:
        return post->boosted();
    case BoostAuthorIdentityRole:
        if (post->boostIdentity()) {
            return QVariant::fromValue<Identity *>(post->boostIdentity().get());
        }
        return false;
    case IsReplyRole:
        return !post->inReplyTo().isEmpty();
    case ReplyAuthorIdentityRole:
        if (!post->inReplyTo().isEmpty()) {
            return QVariant::fromValue<Identity *>(post->replyIdentity().get());
        }
        return false;
    case PublishedAtRole:
        return post->publishedAt();
    case RebloggedRole:
        return post->reblogged();
    case FavouritedRole:
        return post->favourited();
    case BookmarkedRole:
        return post->bookmarked();
    case PinnedRole:
        return post->pinned();

    case FavouritesCountRole:
        return post->favouritesCount();
    case RepliesCountRole:
        return post->repliesCount();
    case ReblogsCountRole:
        return post->reblogsCount();
    case SensitiveRole:
        return post->sensitive();
    case SpoilerTextRole:
        return post->spoilerText();
    case VisibilityRole:
        return post->visibility();
    case WasEditedRole:
        return post->wasEdited();
    case EditedAtRole:
        return post->editedAt();
    case FiltersRole:
        return post->filters();
    case AttachmentsRole:
        return QVariant::fromValue<QList<Attachment *>>(post->attachments());
    case CardRole:
        if (post->card().has_value()) {
            return QVariant::fromValue<Card>(*post->card());
        }
        return false;
    case ApplicationRole:
        if (post->application().has_value()) {
            return QVariant::fromValue<Application>(*post->application());
        }
        return false;
    case UrlRole:
        return QVariant::fromValue<QUrl>(post->url());
    case RelativeTimeRole: {
        return post->relativeTime();
    }
    case AbsoluteTimeRole: {
        return post->absoluteTime();
    }
    case PollRole:
        if (post->poll()) {
            return QVariant::fromValue<Poll>(*post->poll());
        }
        return {};
    case TypeRole:
    case NotificationActorIdentityRole:
        return {};
    case PostRole:
        return QVariant::fromValue<Post *>(post);
    }

    return {};
}

void AbstractTimelineModel::actionFavorite(const QModelIndex &index, Post *post)
{
    if (!post->favourited()) {
        m_account->favorite(post);
        post->setFavourited(true);
    } else {
        m_account->unfavorite(post);
        post->setFavourited(false);
    }

    Q_EMIT dataChanged(index, index, {FavouritedRole});
}

void AbstractTimelineModel::actionRepeat(const QModelIndex &index, Post *post)
{
    if (!post->reblogged()) {
        m_account->repeat(post);
        post->setReblogged(true);
    } else {
        m_account->unrepeat(post);
        post->setReblogged(false);
    }

    Q_EMIT dataChanged(index, index, {RebloggedRole});
}

void AbstractTimelineModel::actionRedraft(const QModelIndex &index, Post *post, bool isEdit)
{
    m_account->get(m_account->apiUrl(QStringLiteral("/api/v1/statuses/%1/source").arg(post->postId())),
                   true,
                   this,
                   [this, post, index, isEdit](QNetworkReply *reply) {
                       const auto postSource = QJsonDocument::fromJson(reply->readAll()).object();

                       auto backend = new PostEditorBackend();
                       backend->setId(post->postId());
                       backend->setStatus(postSource["text"_L1].toString());
                       backend->setSpoilerText(postSource["spoiler_text"_L1].toString());
                       backend->setInReplyTo(post->inReplyTo());
                       backend->setVisibility(post->visibility());
                       backend->setLanguage(post->language());
                       backend->setMentions(post->mentions()); // TODO: needed?
                       backend->setSensitive(post->sensitive());
                       backend->setHasExistingPoll(post->poll() != nullptr);

                       Q_EMIT postSourceReady(backend, isEdit);

                       auto attachmentBackend = backend->attachmentEditorModel();
                       for (const auto &attachment : post->attachments()) {
                           attachmentBackend->appendExisting(attachment);
                       }

                       if (isEdit) {
                           connect(backend, &PostEditorBackend::editComplete, this, [this, post, index](QJsonObject object) {
                               post->fromJson(std::move(object));
                               Q_EMIT dataChanged(index, index);
                           });
                       }
                   });
}

void AbstractTimelineModel::actionBookmark(const QModelIndex &index, Post *post)
{
    if (!post->bookmarked()) {
        m_account->bookmark(post);
        post->setBookmarked(true);
    } else {
        m_account->unbookmark(post);
        post->setBookmarked(false);
    }

    Q_EMIT dataChanged(index, index, {BookmarkedRole});
}

void AbstractTimelineModel::actionPin(const QModelIndex &index, Post *post)
{
    if (!post->pinned()) {
        m_account->pin(post);
        post->setPinned(true);
    } else {
        m_account->unpin(post);
        post->setPinned(false);
    }

    Q_EMIT dataChanged(index, index, {PinnedRole});
}

void AbstractTimelineModel::actionDelete(const QModelIndex &index, Post *post)
{
    Q_UNUSED(index);
    m_account->deleteResource(m_account->apiUrl(QStringLiteral("/api/v1/statuses/%1").arg(post->postId())), true, this, {});
}

#include "moc_abstracttimelinemodel.cpp"