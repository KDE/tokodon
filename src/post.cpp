// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-License-Identifier: GPL-3.0-only

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMap>

#include "account.h"
#include "post.h"

static QMap<QString, Attachment::AttachmentType> str_to_att_type = {{"image", Attachment::AttachmentType::Image},
                                                                    {"gifv", Attachment::AttachmentType::GifV},
                                                                    {"video", Attachment::AttachmentType::Video}};

Attachment::Attachment(Post *parent, QJsonObject &obj)
    : m_parent(parent)
{
    m_type = Unknown;
    if (!obj.contains("type"))
        return;

    m_id = obj["id"].toString();
    m_url = obj["url"].toString();
    m_preview_url = obj["preview_url"].toString();
    m_description = obj["description"].toString();

    // determine type if we can
    auto type = obj["type"].toString();
    if (str_to_att_type.contains(type))
        m_type = str_to_att_type[type];
}

Attachment::~Attachment()
{
}

void Attachment::setDescription(QString desc)
{
    m_description = desc;
    m_parent->updateAttachment(this);
}

static QMap<Post::Visibility, QString> vis_to_str = {{Post::Visibility::Public, "public"},
                                                     {Post::Visibility::Unlisted, "unlisted"},
                                                     {Post::Visibility::Private, "private"},
                                                     {Post::Visibility::Direct, "direct"}};

static QMap<QString, Post::Visibility> str_to_vis = {{"public", Post::Visibility::Public},
                                                     {"unlisted", Post::Visibility::Unlisted},
                                                     {"private", Post::Visibility::Private},
                                                     {"direct", Post::Visibility::Direct}};

Post::Post(Account *parent)
    : QObject(parent)
    , m_parent(parent)
{
    QString vis_str = parent->identity().m_visibility;
    m_visibility = str_to_vis[vis_str];
}

Post::Post(Account *parent, QJsonObject obj)
    : QObject(parent)
    , m_parent(parent)
    , m_visibility(Post::Visibility::Public)
{
    auto account_doc = obj["account"].toObject();
    auto acct = account_doc["acct"].toString();
    auto reblog_obj = obj["reblog"].toObject();

    if (!obj.contains("reblog") || reblog_obj.isEmpty()) {
        m_repeat = false;
        m_author_identity = m_parent->identityLookup(acct, account_doc);
    } else {
        m_repeat = true;

        auto repeat_doc = obj["reblog"].toObject();
        auto repeat_account_doc = repeat_doc["account"].toObject();
        auto repeat_acct = repeat_account_doc["acct"].toString();

        m_author_identity = m_parent->identityLookup(repeat_acct, repeat_account_doc);
        m_repeat_identity = m_parent->identityLookup(acct, account_doc);
    }

    m_subject = obj["spoiler_text"].toString();
    m_content = obj["content"].toString();
    m_post_id = m_replyTargetId = obj["id"].toString();
    m_isFavorite = obj["favourited"].toBool();
    m_favoriteCount = obj["favourites_count"].toInt();
    m_repeatedCount = obj["reblogs_count"].toInt();
    m_repliesCount = obj["replies_count"].toInt();
    m_isRepeated = obj["reblogged"].toBool();
    m_isSensitive = obj["sensitive"].toBool();
    m_link = QUrl(obj["url"].toString());
    m_pinned = obj["pinned"].toBool();
    m_visibility = str_to_vis[obj["visibility"].toString()];
    m_published_at = QDateTime::fromString(obj["created_at"].toString(), Qt::ISODate);

    QJsonArray mentions;

    if (m_repeat) {
        m_visibility = str_to_vis[reblog_obj["visibility"].toString()];
        m_replyTargetId = reblog_obj["id"].toString();
        addAttachments(reblog_obj["media_attachments"].toArray());

        mentions = reblog_obj["mentions"].toArray();
    } else {
        addAttachments(obj["media_attachments"].toArray());

        mentions = obj["mentions"].toArray();
    }

    for (const auto &m : qAsConst(mentions)) {
        const QJsonObject o = m.toObject();
        m_mentions.push_back("@" + o["acct"].toString());
    }

    m_attachments_visible = !m_isSensitive;
}

Post::~Post()
{
    m_attachments.clear();
}

void Post::addAttachments(const QJsonArray &attachments)
{
    for (const auto &attachment_val : attachments) {
        auto attachment_obj = attachment_val.toObject();
        auto attachment = new Attachment(this, attachment_obj);

        m_attachments.push_back(attachment);
    }
}

void Post::setInReplyTo(const QString &inReplyTo)
{
    if (inReplyTo == m_replyTargetId) {
        return;
    }
    m_replyTargetId = inReplyTo;
    Q_EMIT inReplyToChanged();
}

QString Post::inReplyTo() const
{
    return m_replyTargetId;
}

void Post::setDirtyAttachment()
{
    m_parent->invalidatePost(this);
}

QStringList Post::mentions() const
{
    return m_mentions;
}

void Post::setMentions(const QStringList &mentions)
{
    if (mentions == m_mentions) {
        return;
    }
    m_mentions = mentions;
    Q_EMIT mentionsChanged();
}

QJsonDocument Post::toJsonDocument() const
{
    QJsonObject obj;

    obj["spoiler_text"] = m_subject;
    obj["status"] = m_content;
    obj["content_type"] = m_content_type;
    obj["sensitive"] = m_isSensitive;
    obj["visibility"] = vis_to_str[m_visibility];

    if (!m_replyTargetId.isEmpty())
        obj["in_reply_to_id"] = m_replyTargetId;

    auto media_ids = QJsonArray();
    for (const auto att : qAsConst(m_attachments)) {
        media_ids.append(att->m_id);
    }

    obj["media_ids"] = media_ids;

    return QJsonDocument(obj);
}

void Post::uploadAttachment(const QUrl &filename)
{
    QFile *file = new QFile(filename.toLocalFile());
    const QFileInfo info(filename.toLocalFile());

    file->open(QFile::ReadOnly);
    m_parent->upload(this, file, info.fileName());
}

void Post::updateAttachment(Attachment *a)
{
    m_parent->updateAttachment(a);
}

static QMap<QString, Notification::Type> str_to_not_type = {{"favourite", Notification::Type::Favorite},
                                                            {"follow", Notification::Type::Follow},
                                                            {"mention", Notification::Type::Mention},
                                                            {"reblog", Notification::Type::Repeat}};

Notification::Notification(Account *parent, QJsonObject &obj)
    : m_account(parent)
{
    QJsonObject account = obj["account"].toObject();
    QJsonObject status = obj["status"].toObject();
    auto acct = account["acct"].toString();
    auto type = obj["type"].toString();

    m_post = std::make_shared<Post>(m_account, status);
    m_identity = m_account->identityLookup(acct, account);
    m_type = str_to_not_type[type];
    m_id = obj["id"].toString().toInt();
}

int Notification::id() const
{
    return m_id;
}

QString Post::subject() const
{
    return m_subject;
}

void Post::setSubject(const QString &subject)
{
    if (subject == m_subject) {
        return;
    }
    m_subject = subject;
    Q_EMIT subjectChanged();
}

QString Post::content() const
{
    return m_content;
}

void Post::setContent(const QString &content)
{
    if (content == m_content) {
        return;
    }
    m_content = content;
    Q_EMIT contentChanged();
}

QString Post::contentType() const
{
    return m_content_type;
}

void Post::setContentType(const QString &contentType)
{
    if (m_content_type == contentType) {
        return;
    }
    m_content_type = contentType;
    Q_EMIT contentTypeChanged();
}

bool Post::isSensitive() const
{
    return m_isSensitive;
}

void Post::setSensitive(bool isSensitive)
{
    if (m_isSensitive == isSensitive) {
        return;
    }
    m_isSensitive = isSensitive;
    Q_EMIT sensitiveChanged();
}

Post::Visibility Post::visibility() const
{
    return m_visibility;
}

void Post::setVisibility(Visibility visibility)
{
    if (visibility == m_visibility) {
        return;
    }
    m_visibility = visibility;
    Q_EMIT visibilityChanged();
}
