// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "poll.h"
#include "timeline/attachment.h"

#include <QImage>

class Post;
class Identity;
class AbstractAccount;

class Application
{
    Q_GADGET

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QUrl website READ website)

public:
    Application() = default;
    explicit Application(QJsonObject application);

    QString name() const;
    QUrl website() const;

private:
    QJsonObject m_application;
};

class Card
{
    Q_GADGET

    Q_PROPERTY(QString authorName READ authorName)
    Q_PROPERTY(QString authorUrl READ authorUrl)
    Q_PROPERTY(QString blurhash READ blurhash)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QString embedUrl READ embedUrl)
    Q_PROPERTY(int width READ width)
    Q_PROPERTY(int height READ height)
    Q_PROPERTY(QString html READ html)
    Q_PROPERTY(QString image READ image)
    Q_PROPERTY(QString providerName READ providerName)
    Q_PROPERTY(QString providerUrl READ providerUrl)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QUrl url READ url)

public:
    Card() = default;
    explicit Card(QJsonObject card);

    QString authorName() const;
    QString authorUrl() const;
    QString blurhash() const;
    QString description() const;
    QString embedUrl() const;
    int width() const;
    int height() const;
    QString html() const;
    QString image() const;
    QString providerName() const;
    QString providerUrl() const;
    QString title() const;
    QUrl url() const;

private:
    QJsonObject m_card;
};

/**
 * @brief Represents a post, which may have text or images attached.
 */
class Post : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Posts should be accessed via models")

    Q_PROPERTY(QString spoilerText READ spoilerText CONSTANT)
    Q_PROPERTY(QString content READ content CONSTANT)
    Q_PROPERTY(bool sensitive READ sensitive CONSTANT)
    Q_PROPERTY(Visibility visibility READ visibility CONSTANT)
    Q_PROPERTY(QString language READ language CONSTANT)
    Q_PROPERTY(QString inReplyTo READ inReplyTo CONSTANT)
    Q_PROPERTY(QStringList mentions READ mentions CONSTANT)
    Q_PROPERTY(Poll *poll READ poll NOTIFY pollChanged)
    Q_PROPERTY(QStringList filters READ filters CONSTANT)
    Q_PROPERTY(Identity *authorIdentity READ getAuthorIdentity CONSTANT)
    Q_PROPERTY(QQmlListProperty<Attachment> attachments READ attachmentList CONSTANT)
    Q_PROPERTY(QString relativeTime READ relativeTime CONSTANT)
    Q_PROPERTY(QString absoluteTime READ absoluteTime CONSTANT)
    Q_PROPERTY(Card *card READ getCard CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(QString editedAt READ editedAt CONSTANT)
    Q_PROPERTY(bool wasEdited READ wasEdited CONSTANT)
    Q_PROPERTY(QList<QString> standaloneTags READ standaloneTags CONSTANT)

public:
    Post() = delete;
    Post(const Post &) = delete;

    /**
     * @brief Create an empty post for @p account.
     * @note The @c Post is not parented to the account automatically.
     */
    explicit Post(AbstractAccount *account, QObject *parent = nullptr);

    /**
     * @brief Create a post for @p account from JSON @p obj.
     * @note The @c Post is not parented to the account automatically.
     */
    Post(AbstractAccount *account, QJsonObject obj, QObject *parent = nullptr);

    /**
     * @brief Loads post content from JSON @p obj.
     */
    void fromJson(QJsonObject obj);

    /**
     * @return This post's id.
     * @note The id may be different because it was boosted. This is the id of the parent post.
     * @sa originalPostId()
     */
    QString postId() const;

    /**
     * @return The post's original id if boosted, but identical to postId if not.
     * @sa postId()
     */
    QString originalPostId() const;

    /**
     * @return The published/creation time of this post.
     */
    QDateTime publishedAt() const;

    /**
     * @return A locale-aware relative time when the post was published.
     */
    QString relativeTime() const;

    /**
     * @return A absolute locale-aware time when the post was published.
     */
    QString absoluteTime() const;

    /**
     * @return The identity of this post's author.
     */
    std::shared_ptr<Identity> authorIdentity() const;

    /**
     * @return The HTML text of this post.
     */
    QString content() const;

    /**
     * @brief The possible visibility levels of a post.
     */
    enum Visibility { Public, Unlisted, Private, Direct, Local };
    Q_ENUM(Visibility)

    /**
     * @return The visibility of the post.
     */
    Visibility visibility() const;

    /**
     * @return Whether the post has a content warning and should be considered sensitive.
     */
    bool sensitive() const;

    /**
     * @return The spoiler text (subject) of the post.
     */
    QString spoilerText() const;

    /**
     * @return The attachments for this post.
     */
    QList<Attachment *> attachments() const;

    /**
     * @return The application used to write this post, if there is one available.
     */
    std::optional<Application> application() const;

    /**
     * @return The list of username mentions in this post.
     */
    QStringList mentions() const;

    /**
     * @return The standalone tags of this post that are not part of the main text.
     */
    QVector<QString> standaloneTags() const;

    /**
     * @return The number of time this post has been replied.
     */
    int repliesCount() const;

    /**
     * @return The number of time this post has been favorited.
     */
    int favouritesCount() const;

    /**
     * @brief The number of time this post has been boosted.
     */
    int reblogsCount() const;

    /**
     * @return A web accessible URL to this post.
     */
    QUrl url() const;

    /**
     * @return The id that this post is replying to. Could be empty if it isn't replying to anything.
     */
    QString inReplyTo() const;

    /**
     * @return The identity of who replied to this post, if someone did.
     */
    std::shared_ptr<Identity> replyIdentity() const;

    /**
     * @return The poll on this post, if there is one.
     */
    Poll *poll() const;

    /**
     * @brief Sets the poll on this post from JSON @p object.
     */
    void setPollJson(const QJsonObject &object);

    /**
     * @return The link card for this post, if there is one.
     */
    std::optional<Card> card() const;

    /**
     * @return The ISO-639 language code that the post is sent in.
     * @note This doesn't reflect the actual content of the post, and is set by the author.
     */
    QString language() const;

    /**
     * @return If the post was edited.
     */
    bool wasEdited() const;

    /**
     * @return The time this post was last edited
     */
    QString editedAt() const;

    /**
     * @return If this post existed because it was boosted by someone else.
     * @sa boostIdentity()
     */
    bool boosted() const;

    /**
     * @return The identity of who boosted this post, if someone did.
     */
    std::shared_ptr<Identity> boostIdentity() const;

    /**
     * @return Whether the user favorited this post or not.
     */
    bool favourited() const;

    /**
     * @brief Set this post as favourited as or not.
     */
    void setFavourited(bool favourited);

    /**
     * @return Whether the user boosted this post.
     */
    bool reblogged() const;

    /**
     * @brief Set this post as boosted or not.
     */
    void setReblogged(bool reblogged);

    /**
     * @return Whether the user bookmarked this post.
     */
    bool bookmarked() const;

    /**
     * @brief Set this post as bookmarked or not.
     */
    void setBookmarked(bool bookmarked);

    /**
     * @return Whether the user muted this post.
     */
    bool muted() const;

    /**
     * @brief Set this post as muted or not.
     */
    void setMuted(bool muted);

    /**
     * @return The filters that apply to this post.
     */
    QStringList filters() const;

    /**
     * @return Whether this post is supposed to be hidden by a filter.
     */
    bool hidden() const;

    /**
     * @return Whether the user pinned this post.
     */
    bool pinned() const;

    /**
     * @brief Set whether or not this post is pinned to the top of the timeline.
     */
    void setPinned(bool pinned);

    /**
     * @return Whether the user filtered this post.
     */
    bool filtered() const;

    /**
     * @brief Adds @p attachments to this post.
     */
    Q_INVOKABLE void addAttachments(const QJsonArray &attachments);

    /**
     * @return The string representation of the @p visibility enum.
     */
    static QString visibilityToString(Post::Visibility visibility);

    /**
     * @brief The enum that matches the @p visibility string.
     */
    static Post::Visibility stringToVisibility(const QString &visibility);

Q_SIGNALS:
    void pollChanged();
    void replyIdentityChanged();

private:
    QString type() const;

    Identity *getAuthorIdentity() const;

    QQmlListProperty<Attachment> attachmentList() const;

    Card *getCard() const;

    void setCard(std::optional<Card> card);

    void setApplication(std::optional<Application> application);

    void processContent(const QJsonObject &obj);

    AbstractAccount *m_parent;

    QDateTime m_publishedAt;
    QString m_postId;
    QString m_originalPostId;
    QUrl m_url;
    QString m_content;
    QString m_spoilerText;
    QString m_author;
    QString m_reply_to_author;
    QString m_content_type;
    QStringList m_mentions;
    QString m_language;
    QDateTime m_editedAt;
    QVector<QString> m_standaloneTags;

    QString m_replyTargetId;
    QStringList m_filters;
    std::optional<Card> m_card;
    std::optional<Application> m_application;
    std::shared_ptr<Identity> m_authorIdentity;
    QList<Attachment *> m_attachments;
    QQmlListProperty<Attachment> m_attachmentList;
    std::unique_ptr<Poll> m_poll;

    bool m_sensitive = false;
    Visibility m_visibility;

    bool m_boosted = false;
    std::shared_ptr<Identity> m_boostIdentity;

    std::shared_ptr<Identity> m_replyIdentity;

    bool m_favourited = false;
    bool m_reblogged = false;
    bool m_muted = false;
    bool m_bookmarked = false;
    bool m_filtered = false;
    bool m_hidden = false;
    bool m_pinned = false;

    int m_reblogsCount = 0;
    int m_favouritesCount = 0;
    int m_repliesCount = 0;
};
