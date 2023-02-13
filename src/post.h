// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QDateTime>
#include <QImage>
#include <QJsonObject>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QUrl>

#include <QJsonObject>
#include <memory>
#include <optional>

class Post;
class Identity;
class AbstractAccount;
class Poll;

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
    Card(QJsonObject card);

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

/// Post's attachment object.
/// TODO make it possible to fetch the images with a Qml image provider.
/// TODO use getter and setter
class Attachment : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id MEMBER m_id CONSTANT)
    Q_PROPERTY(AttachmentType attachmentType MEMBER m_type CONSTANT)
    Q_PROPERTY(QString previewUrl MEMBER m_preview_url CONSTANT)
    Q_PROPERTY(QString url MEMBER m_url CONSTANT)
    Q_PROPERTY(QString remoteUrl MEMBER m_remote_url CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString blurhash MEMBER m_blurhash CONSTANT)
    Q_PROPERTY(int originalWidth MEMBER m_originalWidth CONSTANT)
    Q_PROPERTY(int originalHeight MEMBER m_originalHeight CONSTANT)

public:
    explicit Attachment(QObject *parent = nullptr);
    explicit Attachment(const QJsonObject &object, QObject *parent = nullptr);

    enum AttachmentType {
        Unknown,
        Image,
        GifV,
        Video,
    };
    Q_ENUM(AttachmentType);

    Post *m_parent;

    QString m_id;
    AttachmentType m_type;
    QString m_preview_url;
    QString m_url;
    QString m_remote_url;
    QString m_blurhash;
    int m_originalWidth;
    int m_originalHeight;

    QString id() const;

    void setDescription(const QString &description);
    QString description() const;

private:
    void fromJson(const QJsonObject &object);

    QString m_description;
};

class Notification
{
    Q_GADGET

public:
    Notification() = default;
    explicit Notification(AbstractAccount *account, const QJsonObject &obj, QObject *parent = nullptr);

    enum Type {
        Mention,
        Follow,
        Repeat,
        Favorite,
        Poll,
        FollowRequest,
        Update,
    };
    Q_ENUM(Type);

    int id() const;
    AbstractAccount *account() const;
    Type type() const;
    Post *post() const;
    std::shared_ptr<Identity> identity() const;

private:
    int m_id;

    AbstractAccount *m_account = nullptr;
    Post *m_post = nullptr;
    Type m_type;
    std::shared_ptr<Identity> m_identity;
};

class Post : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString spoilerText READ spoilerText WRITE setSpoilerText NOTIFY spoilerTextChanged)
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(bool sensitive READ sensitive WRITE setSensitive NOTIFY sensitiveChanged)
    Q_PROPERTY(Visibility visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString inReplyTo READ inReplyTo WRITE setInReplyTo NOTIFY inReplyToChanged)
    Q_PROPERTY(QStringList mentions READ mentions WRITE setMentions NOTIFY mentionsChanged)
    Q_PROPERTY(Poll *poll READ poll NOTIFY pollChanged)
    Q_PROPERTY(QStringList filters READ filters CONSTANT)

public:
    Post() = delete;
    Post(const Post &) = delete;
    Post(AbstractAccount *account, QObject *parent = nullptr);
    Post(AbstractAccount *account, QJsonObject obj, QObject *parent = nullptr);
    ~Post();

    enum Visibility {
        Public,
        Unlisted,
        Private,
        Direct,
    };
    Q_ENUM(Visibility)

    AbstractAccount *m_parent;

    void fromJson(QJsonObject obj);

    std::shared_ptr<Identity> authorIdentity() const;
    std::shared_ptr<Identity> boostIdentity() const;
    bool boosted() const;

    /// Returns the post id of the status itself
    QString postId() const;
    QUrl url() const;

    /// Returns the spoiler text (subject) of the status
    QString spoilerText() const;
    void setSpoilerText(const QString &spoilerText);

    QString content() const;
    void setContent(const QString &content);
    QString contentType() const;
    void setContentType(const QString &contentType);
    bool sensitive() const;
    void setSensitive(bool isSensitive);
    Visibility visibility() const;
    void setVisibility(Visibility visibility);
    QString language() const;
    void setLanguage(const QString &language);
    QString inReplyTo() const;
    void setInReplyTo(const QString &inReplyTo);
    QStringList mentions() const;
    void setMentions(const QStringList &mentions);
    std::optional<Card> card() const;
    void setCard(std::optional<Card> card);
    void setPoll(Poll *poll);
    Poll *poll() const;
    QStringList filters() const;

    void addAttachment(const QJsonObject &attachment);

    /// Returns the published/creation time of this status.
    QDateTime publishedAt() const;

    /// Returns whether the user favorited this status.
    bool favourited() const;
    /// Set this post as favourited.
    void setFavourited(bool favourited);
    /// Returns whether the user reblogged this status.
    bool reblogged() const;
    /// Set this post as reblogged.
    void setReblogged(bool reblogged);
    /// Returns whether the user muted this status.
    bool muted() const;
    /// Set this post as muted.
    void setMuted(bool muted);
    /// Returns whether the user bookmarked this status.
    bool bookmarked() const;
    /// Set this post as bookmarked.
    void setBookmarked(bool bookmarked);
    /// Returns whether the user pinned this status.
    bool pinned() const;
    /// Set this status as pinned
    void setPinned(bool pinned);
    /// Returns whether the user filtered this status.
    bool filtered() const;

    /// Returns the number of time this status has been boosted.
    int reblogsCount() const;
    /// Returns the number of time this status has been favorited.
    int favouritesCount() const;
    /// Returns the number of time this status has been replied.
    int repliesCount() const;

    /// Returns whether this status is empty
    bool isEmpty() const;

    Q_INVOKABLE void addAttachments(const QJsonArray &attachments);
    void setDirtyAttachment();
    void updateAttachment(Attachment *a);
    QVector<Attachment *> attachments() const;
    bool attachmentsVisible() const;
    void setAttachmentsVisible(bool attachmentsVisible);

Q_SIGNALS:
    void spoilerTextChanged();
    void contentChanged();
    void contentTypeChanged();
    void sensitiveChanged();
    void visibilityChanged();
    void languageChanged();
    void inReplyToChanged();
    void mentionsChanged();
    void attachmentUploaded();
    void pollChanged();

private:
    bool m_attachments_visible = true;
    QDateTime m_publishedAt;
    QString m_postId;
    QUrl m_url;
    QString m_content;
    QString m_spoilerText;
    QString m_author;
    QString m_reply_to_author;
    QString m_content_type;
    QStringList m_mentions;
    QString m_language;

    QString m_replyTargetId;
    QStringList m_filters;
    std::optional<Card> m_card;
    std::shared_ptr<Identity> m_authorIdentity;
    QVector<Attachment *> m_attachments;
    Poll *m_poll = nullptr;

    bool m_sensitive;
    Visibility m_visibility;

    bool m_boosted;
    std::shared_ptr<Identity> m_boostIdentity;

    bool m_favourited;
    bool m_reblogged;
    bool m_muted;
    bool m_bookmarked;
    bool m_filtered;
    bool m_pinned;

    int m_reblogsCount;
    int m_favouritesCount;
    int m_repliesCount;
};

Q_DECLARE_METATYPE(Card)
Q_DECLARE_METATYPE(Notification)
