// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-License-Identifier: GPL-3.0-only

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <qurl.h>

#include "account.h"
#include "post.h"

static QMap<QString, Attachment::AttachmentType> stringToAttachmentType = {
    {"image", Attachment::AttachmentType::Image},
    {"gifv", Attachment::AttachmentType::GifV},
    {"video", Attachment::AttachmentType::Video},
};

Attachment::Attachment(Post *parent, const QJsonObject &obj)
    : m_parent(parent)
{
    m_type = Unknown;
    if (!obj.contains("type")) {
        return;
    }

    m_id = obj["id"].toString();
    m_url = obj["url"].toString();
    m_preview_url = obj["preview_url"].toString();
    m_description = obj["description"].toString();
    m_blurhash = obj["blurhash"].toString();

    // determine type if we can
    auto type = obj["type"].toString();
    if (stringToAttachmentType.contains(type)) {
        m_type = stringToAttachmentType[type];
    }
}

Attachment::~Attachment() = default;

void Attachment::setDescription(const QString &description)
{
    m_description = description;
    m_parent->updateAttachment(this);
}

static QMap<Post::Visibility, QString> visibilityToString = {
    {Post::Visibility::Public, "public"},
    {Post::Visibility::Unlisted, "unlisted"},
    {Post::Visibility::Private, "private"},
    {Post::Visibility::Direct, "direct"},
};

static QMap<QString, Post::Visibility> stringToVisibility = {
    {"public", Post::Visibility::Public},
    {"unlisted", Post::Visibility::Unlisted},
    {"private", Post::Visibility::Private},
    {"direct", Post::Visibility::Direct},
};

Post::Post(AbstractAccount *account, QObject *parent)
    : QObject(parent)
    , m_parent(account)
{
    QString visibilityString = account->identity().visibility();
    m_visibility = stringToVisibility[visibilityString];
}

Post::Post(AbstractAccount *account, QJsonObject obj, QObject *parent)
    : QObject(parent)
    , m_parent(account)
    , m_visibility(Post::Visibility::Public)
{
    const auto account_doc = obj["account"].toObject();
    const auto acct = account_doc["acct"].toString();
    const auto reblog_obj = obj["reblog"].toObject();

    m_original_post_id = obj["id"].toString();

    if (!obj.contains("reblog") || reblog_obj.isEmpty()) {
        m_repeat = false;
        m_authorIdentity = m_parent->identityLookup(acct, account_doc);
    } else {
        m_repeat = true;

        auto repeat_account_doc = reblog_obj["account"].toObject();
        auto repeat_acct = repeat_account_doc["acct"].toString();

        m_authorIdentity = m_parent->identityLookup(repeat_acct, repeat_account_doc);
        m_repeatIdentity = m_parent->identityLookup(acct, account_doc);

        obj = reblog_obj;
    }

    m_subject = obj["spoiler_text"].toString();
    m_content = obj["content"].toString();

    const auto emojis = obj["emojis"].toArray();

    for (const auto &emoji : emojis) {
        const auto emojiObj = emoji.toObject();
        m_content = m_content.replace(QLatin1Char(':') + emojiObj["shortcode"].toString() + QLatin1Char(':'), "<img height=\"16\" width=\"16\" src=\"" + emojiObj["static_url"].toString() + "\">");
    }

    m_post_id = m_replyTargetId = obj["id"].toString();
    m_isFavorite = obj["favourited"].toBool();
    m_favoriteCount = obj["favourites_count"].toInt();
    m_repeatedCount = obj["reblogs_count"].toInt();
    m_repliesCount = obj["replies_count"].toInt();
    m_isRepeated = obj["reblogged"].toBool();
    m_isSensitive = obj["sensitive"].toBool();
    m_link = QUrl(obj["url"].toString());
    m_pinned = obj["pinned"].toBool();
    m_visibility = stringToVisibility[obj["visibility"].toString()];
    m_published_at = QDateTime::fromString(obj["created_at"].toString(), Qt::ISODate);
    addAttachments(obj["media_attachments"].toArray());
    const QJsonArray mentions = obj["mentions"].toArray();
    if (obj.contains("card") && !obj["card"].toObject().empty()) {
        setCard(std::make_optional<Card>(obj["card"].toObject()));
    }

    for (const auto &m : qAsConst(mentions)) {
        const QJsonObject o = m.toObject();
        m_mentions.push_back("@" + o["acct"].toString());
    }

    m_attachments_visible = !m_isSensitive;
}

Post::~Post()
{
    qDeleteAll(m_attachments);
}

void Post::addAttachments(const QJsonArray &attachments)
{
    for (const auto &attachment_val : attachments) {
        auto attachment_obj = attachment_val.toObject();
        auto attachment = new Attachment(this, attachment_obj);

        m_attachments.push_back(attachment);
    }
}

void Post::addAttachment(const QJsonObject &attachment)
{
    auto att = new Attachment(this, attachment);
    if (att->m_url.isEmpty()) {
        return;
    }

    m_attachments.append(att);

    Q_EMIT attachmentUploaded();
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
    obj["visibility"] = visibilityToString[m_visibility];

    if (!m_replyTargetId.isEmpty()) {
        obj["in_reply_to_id"] = m_replyTargetId;
    }

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

static QMap<QString, Notification::Type> str_to_not_type = {
    {"favourite", Notification::Type::Favorite},
    {"follow", Notification::Type::Follow},
    {"mention", Notification::Type::Mention},
    {"reblog", Notification::Type::Repeat},
};

Notification::Notification(AbstractAccount *account, const QJsonObject &obj, QObject *parent)
    : m_account(account)
{
    const QJsonObject accountObj = obj["account"].toObject();
    const QJsonObject status = obj["status"].toObject();
    auto acct = accountObj["acct"].toString();
    auto type = obj["type"].toString();

    m_post = new Post(m_account, status, parent);
    m_identity = m_account->identityLookup(acct, accountObj);
    m_type = str_to_not_type[type];
    m_id = obj["id"].toString().toInt();
}

int Notification::id() const
{
    return m_id;
}

AbstractAccount *Notification::account() const
{
    return m_account;
}

Notification::Type Notification::type() const
{
    return m_type;
}

Post *Notification::post() const
{
    return m_post;
}

std::shared_ptr<Identity> Notification::identity() const
{
    return m_identity;
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

std::optional<Card> Post::card() const
{
    return m_card;
}

void Post::setCard(std::optional<Card> card)
{
    m_card = card;
}

Card::Card(QJsonObject card)
    : m_card(card)
{
}

QString Card::authorName() const
{
    return m_card[QLatin1String("author_name")].toString();
}

QString Card::authorUrl() const
{
    return m_card[QLatin1String("author_url")].toString();
}

QString Card::blurhash() const
{
    return m_card[QLatin1String("blurhash")].toString();
}

QString Card::description() const
{
    return m_card[QLatin1String("description")].toString();
}

QString Card::embedUrl() const
{
    return m_card[QLatin1String("embed_url")].toString();
}

int Card::width() const
{
    return m_card[QLatin1String("weight")].toInt();
}

int Card::height() const
{
    return m_card[QLatin1String("height")].toInt();
}

QString Card::html() const
{
    return m_card[QLatin1String("html")].toString();
}

QString Card::image() const
{
    return m_card[QLatin1String("image")].toString();
}

QString Card::providerName() const
{
    const auto providerName = m_card[QLatin1String("provider_name")].toString();
    if (!providerName.isEmpty()) {
        return providerName;
    }
    return url().host();
}

QString Card::providerUrl() const
{
    return m_card[QLatin1String("provider_url")].toString();
}

QString Card::title() const
{
    return m_card[QLatin1String("title")].toString().trimmed();
}

QUrl Card::url() const
{
    return QUrl::fromUserInput(m_card[QLatin1String("url")].toString());
}

std::shared_ptr<Identity> Post::authorIdentity() const
{
    return m_authorIdentity;
}

std::shared_ptr<Identity> Post::repeatIdentity() const
{
    return m_repeatIdentity;
}
