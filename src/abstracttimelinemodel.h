// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "accountmanager.h"
#include "post.h"
#include <QAbstractListModel>
#include <qabstractitemmodel.h>

class AbstractAccount;
class PostEditorBackend;

/// Base class for most timeline classes such as the home and notification timelines
/// \see TimelineModel
/// \see NotificationTimeline
class AbstractTimelineModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
public:
    enum CustoRoles {
        IdRole = Qt::UserRole + 1, ///< Post id
        OriginalIdRole, ///< Original post id (boosted posts generate their own id and live in IdRole)
        UrlRole, ///< Original URL of the post, can be from a different instance
        ContentRole, ///< Content text of the post
        SpoilerTextRole, ///< Spoiler label for the post
        AuthorIdentityRole, ///< Identity of the author
        PublishedAtRole, ///< Date that the post was published at
        RelativeTimeRole, ///< Human-readable and locale-aware relative time of the original post date
        AbsoluteTimeRole, ///< Human-readable and locale-aware absolute time of the original post date
        SensitiveRole, ///< Whether or not the post is marked as sensitive
        VisibilityRole, ///< The visibility of the post

        // Additional content
        AttachmentsRole, ///< Media attachments for the post, which can be null
        CardRole, ///< Card for the post, which can be null
        ApplicationRole, ///< Application used for publishing the post
        PollRole, ///< Poll for the post, which can be null
        MentionsRole, ///< List of mentions in the post

        // Reblog
        IsBoostedRole, ///< Does this post show up because it's boosted?
        BoostAuthorIdentityRole, ///< If this is boosted, the identity which boosted it and can be null

        // Reply
        IsReplyRole, ///< Is this post a reply to someone?
        ReplyAuthorIdentityRole, ///< If this is a reply, the identity of the account this post is replying to

        // Interaction count
        ReblogsCountRole, ///< Number of accounts who boosted this post
        RepliesCountRole, ///< Number of accounts who replied to this post
        FavouritesCountRole, ///< Number of accounts who favorited this post

        // User self interaction
        FavouritedRole, ///< Did your own account favorite this post?
        RebloggedRole, ///< Has your own account boosted this post?
        MutedRole, ///< Has your own account muted this post?
        BookmarkedRole, ///< Has your own account bookmarked this post?
        PinnedRole, ///< Is this a pinned post on an account?

        // Notification
        NotificationActorIdentityRole, ///< The identity of account related to this notification
        TypeRole, ///< The notification type

        SelectedRole, ///< Used in ThreadModel. Is this post the selected (or root) post?
        FiltersRole, ///< The filters that may have hidden this post

        PostRole, ///< The original Post object

        ExtraRole, ///< Base role for sub-class roles
    };

    AbstractTimelineModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;

    /// Return if the timeline is still loading
    /// \see setLoading
    bool loading() const;

    /// Set the loading status of the timeline
    /// \see loading
    /// \see loadingChanged
    void setLoading(bool loading);

    /// Favorite the \p post at \p index
    void actionFavorite(const QModelIndex &index, Post *post);

    /// Boost the \p post at \p index
    void actionRepeat(const QModelIndex &index, Post *post);

    /// Delete and re-draft the \p post at \p index
    /// \see postSourceReady
    void actionRedraft(const QModelIndex &index, Post *post, bool isEdit);

    /// Delete the \p post at \p index
    /// May have no effect if the account does not have permission to delete the post
    void actionDelete(const QModelIndex &index, Post *post);

Q_SIGNALS:
    /// Emitted when the timeline loading status has changed
    void loadingChanged();

    /// Emitted when a redraft is requested and the original post source has been fetched
    /// \see actionRedraft
    void postSourceReady(PostEditorBackend *backend, bool isEdit);

protected:
    QVariant postData(Post *post, int role) const;

    AbstractAccount *m_account = nullptr;
    bool m_loading = false;
};
