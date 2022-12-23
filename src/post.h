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
    Q_PROPERTY(QString description MEMBER m_description CONSTANT)
    Q_PROPERTY(QString blurhash MEMBER m_blurhash CONSTANT)
    Q_PROPERTY(int originalWidth MEMBER m_originalWidth CONSTANT)
    Q_PROPERTY(int originalHeight MEMBER m_originalHeight CONSTANT)

public:
    explicit Attachment(Post *parent, const QJsonObject &obj);
    ~Attachment();

    enum AttachmentType { Unknown, Image, GifV, Video };
    Post *m_parent;

    QString m_id;
    AttachmentType m_type;
    QString m_preview_url;
    QString m_url;
    QString m_description;
    QString m_blurhash;
    int m_originalWidth;
    int m_originalHeight;

    void setDescription(const QString &description);
};

class Notification
{
    Q_GADGET

public:
    Notification()
    {
    }
    Notification(const Notification &)
    {
    }
    Notification(AbstractAccount *account, const QJsonObject &obj, QObject *parent = nullptr);

    enum Type {
        Mention,
        Follow,
        Repeat,
        Favorite,
        Poll,
        FollowRequest,
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
    Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY subjectChanged)
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(bool isSensitive READ isSensitive WRITE setSensitive NOTIFY sensitiveChanged)
    Q_PROPERTY(Visibility visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QString inReplyTo READ inReplyTo WRITE setInReplyTo NOTIFY inReplyToChanged)
    Q_PROPERTY(QStringList mentions READ mentions WRITE setMentions NOTIFY mentionsChanged)
    Q_PROPERTY(Poll *poll READ poll NOTIFY pollChanged)

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

    std::shared_ptr<Identity> authorIdentity() const;
    std::shared_ptr<Identity> repeatIdentity() const;

    QString subject() const;
    void setSubject(const QString &subject);
    QString content() const;
    void setContent(const QString &content);
    QString contentType() const;
    void setContentType(const QString &contentType);
    bool isSensitive() const;
    void setSensitive(bool isSensitive);
    Visibility visibility() const;
    void setVisibility(Visibility visibility);
    QString inReplyTo() const;
    void setInReplyTo(const QString &inReplyTo);
    QStringList mentions() const;
    void setMentions(const QStringList &mentions);
    std::optional<Card> card() const;
    void setCard(std::optional<Card> card);
    void setPoll(Poll *poll);
    Poll *poll() const;
    int repliesCount() const;
    QString postId() const;

    void addAttachment(const QJsonObject &attachment);
    Q_INVOKABLE void uploadAttachment(const QUrl &filename);

    bool m_repeat = false;
    int m_repliesCount = 0;
    bool m_isRepeated = false;
    int m_repeatedCount = 0;
    bool m_isFavorite = false;
    int m_favoriteCount = 0;
    bool m_isSensitive = false;
    bool m_attachments_visible = true;

    QDateTime m_published_at;
    QString m_post_id;
    QString m_original_post_id;
    QUrl m_link;
    QString m_content;
    QString m_subject;
    QString m_author;
    QString m_reply_to_author;
    QString m_content_type;
    QList<Attachment *> m_attachments;
    Visibility m_visibility;
    QStringList m_mentions;
    bool m_pinned = false;

    bool isEmpty()
    {
        return m_post_id.isEmpty();
    }
    Q_INVOKABLE void addAttachments(const QJsonArray &attachments);
    void setDirtyAttachment();
    void updateAttachment(Attachment *a);

    // prepares a post for posting
    QJsonDocument toJsonDocument() const;

Q_SIGNALS:
    void subjectChanged();
    void contentChanged();
    void contentTypeChanged();
    void sensitiveChanged();
    void visibilityChanged();
    void inReplyToChanged();
    void mentionsChanged();
    void attachmentUploaded();
    void pollChanged();

private:
    QString m_replyTargetId;
    std::optional<Card> m_card;
    std::shared_ptr<Identity> m_authorIdentity;
    std::shared_ptr<Identity> m_repeatIdentity;
    Poll *m_poll = nullptr;
};

Q_DECLARE_METATYPE(Card)
Q_DECLARE_METATYPE(Notification)
