// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QDateTime>
#include <QUrl>
#include <QString>
#include <QJsonObject>
#include <QImage>
#include <QNetworkReply>

#include <memory>

struct Post;
struct Identity;
class Account;

struct Attachment
{
    Attachment(Post *parent, QJsonObject &obj);
    ~Attachment();

    enum AttachmentType {
        Unknown,
        Image,
        GifV,
        Video
    };

    Post *m_parent;

    QString m_id;
    AttachmentType m_type;
    QString m_preview_url;
    QString m_url;
    QString m_description;

    QImage m_preview;
    QImage m_scratch;

    QNetworkReply *m_reply;

    void fetchPreviewImage();
    void cancelFetchRequest();
    void setDescription(QString desc);
};

struct Notification
{
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
};

struct Post
{
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
        NMembers
    };

    Account *m_parent;
    std::shared_ptr<Identity> m_author_identity;
    std::shared_ptr<Identity> m_repeat_identity;

    bool m_repeat;
    bool m_is_repeated;
    bool m_is_favorited;
    bool m_is_sensitive;
    bool m_is_expanded;
    bool m_attachments_visible;

    QDateTime m_published_at;
    QString m_post_id;
    QString m_reply_target_id;
    QUrl m_link;
    QString m_content;
    QString m_subject;
    QString m_author;
    QString m_reply_to_author;
    QString m_content_type;
    QList<std::shared_ptr<Attachment>> m_attachments;
    Visibility m_visibility;
    QStringList m_mentions;

    bool isEmpty() { return m_post_id.isEmpty(); }
    void addAttachments(const QJsonArray& attachments);
    void setDirtyAttachment();
    void fetchAttachmentPreviews();
    void updateAttachment(Attachment *a);

    // prepares a post for posting
    QJsonDocument toJsonDocument() const;
};
