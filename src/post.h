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

#include <memory>

class Post;
class Identity;
class Account;

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

    void setDescription(QString desc);
};

struct Notification {
    Notification() = delete;
    Notification(const Notification &) = delete;
    Notification(Account *parent, QJsonObject &obj);

    enum Type {
        Mention,
        Follow,
        Repeat,
        Favorite,
    };

    Account *m_account;
    Type m_type;

    std::shared_ptr<Post> m_post;
    std::shared_ptr<Identity> m_identity;
    int id() const;

private:
    int m_id;
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

public:
    Post() = delete;
    Post(const Post &) = delete;
    Post(Account *parent);
    Post(Account *parent, QJsonObject obj);
    ~Post();

    enum Visibility {
        Public,
        Unlisted,
        Private,
        Direct,
    };
    Q_ENUM(Visibility)

    Account *m_parent;
    std::shared_ptr<Identity> m_author_identity;
    std::shared_ptr<Identity> m_repeat_identity;

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

private:
    QString m_replyTargetId;
};
