// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "account/accountmanager.h"

class AbstractAccount;
class PostEditorBackend;

/**
 * @brief Base class for most timeline classes such as the home and notification timelines.
 * @see TimelineModel
 * @see NotificationTimeline
 */
class AbstractTimelineModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool atEnd READ atEnd NOTIFY atEndChanged)

public:
    enum CustoRoles {
        IdRole = Qt::UserRole + 1, /** Post id. */
        OriginalIdRole, /** Original post id (boosted posts generate their own id and live in IdRole) */
        UrlRole, /** Original URL of the post, can be from a different instance. */
        ContentRole, /** Content text of the post. */
        SpoilerTextRole, /** Spoiler label for the post. */
        AuthorIdentityRole, /** Identity of the author. */
        PublishedAtRole, /** Date that the post was published at. */
        RelativeTimeRole, /** Human-readable and locale-aware relative time of the original post date. */
        AbsoluteTimeRole, /** Human-readable and locale-aware absolute time of the original post date. */
        SensitiveRole, /** Whether or not the post is marked as sensitive. */
        VisibilityRole, /** The visibility of the post. */
        WasEditedRole, /** If the post was edited at all. */
        EditedAtRole, /** The datetime of the last edit. */
        IsThreadReplyRole, /** True if this reply in the thread is not for the main thread. */
        IsLastThreadReplyRole, /** True if this reply is the last "thread reply" and should be visually separated. */

        // Additional content
        AttachmentsRole, /** Media attachments for the post, which can be null. */
        CardRole, /** Card for the post, which can be null. */
        ApplicationRole, /** Application used for publishing the post. */
        PollRole, /** Poll for the post, which can be null. */
        MentionsRole, /** List of mentions in the post. */

        // Reblog
        IsBoostedRole, /** Does this post show up because it's boosted? */
        BoostAuthorIdentityRole, /** If this is boosted, the identity which boosted it and can be null. */

        // Reply
        IsReplyRole, /** Is this post a reply to someone? */
        ReplyAuthorIdentityRole, /** If this is a reply, the identity of the account this post is replying to. */

        // Interaction count
        ReblogsCountRole, /** Number of accounts who boosted this post. */
        RepliesCountRole, /** Number of accounts who replied to this post. */
        FavouritesCountRole, /** Number of accounts who favorited this post. */
        QuotesCountRole, /** Number of quotes of this post. */

        // User self interaction
        FavouritedRole, /** Did your own account favorite this post? */
        RebloggedRole, /** Has your own account boosted this post? */
        MutedRole, /** Has your own account muted this post? */
        BookmarkedRole, /** Has your own account bookmarked this post? */
        PinnedRole, /** Is this a pinned post on an account? */

        // Notification
        NotificationActorIdentityRole, /** The identity of account related to this notification. */
        TypeRole, /** The notification type. */
        IsGroupRole,
        IsInGroupRole,
        NumInGroupRole,

        SelectedRole, /** Used in ThreadModel. Is this post the selected (or root) post? */
        FiltersRole, /** The filters that may have hidden this post. */

        PostRole, /** The original Post object. */

        ShowReadMarkerRole, /** Show the read marker above this post */

        ExtraRole, /** Base role for sub-class roles. */
    };

    explicit AbstractTimelineModel(QObject *parent = nullptr);

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    /**
     * @return If the timeline is still loading.
     * @see setLoading()
     */
    [[nodiscard]] virtual bool loading() const;

    /**
     * @brief Set the loading status of the timeline.
     * @see loading()
     * @see loadingChanged()
     */
    void setLoading(bool loading);

    /**
     * @brief Favorite the @p post at @p index.
     */
    void actionFavorite(const QModelIndex &index, Post *post);

    /**
     * @brief Boost the @p post at @p index.
     */
    void actionRepeat(const QModelIndex &index, Post *post);

    /**
     * @brief Delete and re-draft the @p post at @p index.
     * @see postSourceReady()
     */
    void actionRedraft(const QModelIndex &index, Post *post, bool isEdit);

    /**
     * @brief Delete the @p post at @p index.
     * @note May have no effect if the account does not have permission to delete the post.
     */
    void actionDelete(const QModelIndex &index, Post *post);

    /**
     * @brief Bookmark the @p post at @p index.
     */
    void actionBookmark(const QModelIndex &index, Post *post);

    /**
     * @brief Pin the @p post at @p index.
     */
    void actionPin(const QModelIndex &index, Post *post);

    /**
     * @brief Mute this @p post's conversation at @p index.
     */
    void actionMute(const QModelIndex &index, Post *post);

    /**
     * @brief Refresh the timeline.
     * For example, refreshing a thread should load any new replies and statistics.
     */
    virtual void refresh() { };

    virtual bool atEnd() const
    {
        return false;
    }

Q_SIGNALS:
    /**
     * @brief Emitted when the timeline loading status has changed.
     */
    void loadingChanged();

    /**
     * @brief Emitted when a redraft is requested and the original post source has been fetched
     * @see actionRedraft()
     */
    void postSourceReady(PostEditorBackend *backend, bool isEdit);

    /**
     * @brief Emitted when a network error occured when loading this timeline.
     */
    void networkErrorOccurred(const QString &message);

    void atEndChanged();

protected:
    QVariant postData(Post *post, int role) const;

    AbstractAccount *m_account = nullptr;
    bool m_loading = false;
};
