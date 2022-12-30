// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-License-Identifier: GPL-3.0-only

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <qurl.h>

#include "account.h"
#include "poll.h"
#include "post.h"
#include "utils.h"

static QMap<QString, Attachment::AttachmentType> stringToAttachmentType = {
    {"image", Attachment::AttachmentType::Image},
    {"gifv", Attachment::AttachmentType::GifV},
    {"video", Attachment::AttachmentType::Video},
    {"unknown", Attachment::AttachmentType::Unknown},
};

Attachment::Attachment(const QJsonObject &obj)
{
    fromJson(obj);
}

Attachment::~Attachment() = default;

void Attachment::fromJson(const QJsonObject &obj)
{
    if (!obj.contains("type")) {
        m_type = Unknown;
        return;
    }

    m_id = obj["id"].toString();
    m_url = obj["url"].toString();
    m_preview_url = obj["preview_url"].toString();
    m_remote_url = obj["remote_url"].toString();

    setDescription(obj["description"].toString());
    m_blurhash = obj["blurhash"].toString();
    m_originalHeight = obj["meta"].toObject()["original"].toObject()["height"].toInt();
    m_originalWidth = obj["meta"].toObject()["original"].toObject()["width"].toInt();

    // determine type if we can
    const auto type = obj["type"].toString();
    if (stringToAttachmentType.contains(type)) {
        m_type = stringToAttachmentType[type];
    }
}

QString Attachment::description() const
{
    return m_description;
}

void Attachment::setDescription(const QString &description)
{
    m_description = description;
}

QString Attachment::id() const
{
    return m_id;
}

Post::Post(AbstractAccount *account, QObject *parent)
    : QObject(parent)
    , m_parent(account)
{
    QString visibilityString = account->identity()->visibility();
    m_visibility = stringToVisibility(visibilityString);
}

QString computeContent(const QJsonObject &obj, std::shared_ptr<Identity> authorIdentity)
{
    QString content = obj["content"].toString();
    const auto emojis = obj["emojis"].toArray();

    for (const auto &emoji : emojis) {
        const auto emojiObj = emoji.toObject();
        content = content.replace(QLatin1Char(':') + emojiObj["shortcode"].toString() + QLatin1Char(':'),
                                  "<img height=\"16\" align=\"middle\" width=\"16\" src=\"" + emojiObj["static_url"].toString() + "\">");
    }

    const auto tags = obj["tags"].toArray();
    const QString baseUrl = authorIdentity->url().toDisplayString(QUrl::RemovePath);

    for (const auto &tag : tags) {
        const auto tagObj = tag.toObject();
        content = content.replace(baseUrl + QStringLiteral("/tags/") + tagObj["name"].toString(),
                                  QStringLiteral("hashtag:/") + tagObj["name"].toString(),
                                  Qt::CaseInsensitive);
    }

    return content;
}

Post::Post(AbstractAccount *account, QJsonObject obj, QObject *parent)
    : QObject(parent)
    , m_parent(account)
    , m_visibility(Post::Visibility::Public)
{
    const auto accountDoc = obj["account"].toObject();
    const auto accountId = accountDoc["id"].toString();

    m_postId = obj["id"].toString();
    const auto reblogObj = obj["reblog"].toObject();

    if (!obj.contains("reblog") || reblogObj.isEmpty()) {
        m_boosted = false;
        m_authorIdentity = m_parent->identityLookup(accountId, accountDoc);
    } else {
        m_boosted = true;

        const auto reblogAccountDoc = reblogObj["account"].toObject();
        const auto reblogAccountId = reblogAccountDoc["id"].toString();

        m_authorIdentity = m_parent->identityLookup(reblogAccountId, reblogAccountDoc);
        m_boostIdentity = m_parent->identityLookup(accountId, accountDoc);

        obj = reblogObj;
    }

    m_spoilerText = obj["spoiler_text"].toString();
    m_content = computeContent(obj, m_authorIdentity);

    m_replyTargetId = obj["id"].toString();
    m_url = QUrl(obj["url"].toString());

    m_favouritesCount = obj["favourites_count"].toInt();
    m_reblogsCount = obj["reblogs_count"].toInt();
    m_repliesCount = obj["replies_count"].toInt();

    m_favourited = obj["favourited"].toBool();
    m_reblogged = obj["reblogged"].toBool();
    m_bookmarked = obj["bookmarked"].toBool();
    m_pinned = obj["pinned"].toBool();
    m_muted = obj["muted"].toBool();
    const auto filters = obj["filtered"].toArray();
    for (const auto &filter : filters) {
        const auto filterContext = filter.toObject();
        const auto filterObj = filterContext["filter"].toObject();
        m_filters << filterObj["title"].toString();
        m_filtered = true;
    }

    m_sensitive = obj["sensitive"].toBool();
    m_visibility = stringToVisibility(obj["visibility"].toString());

    m_publishedAt = QDateTime::fromString(obj["created_at"].toString(), Qt::ISODate).toLocalTime();
    addAttachments(obj["media_attachments"].toArray());
    const QJsonArray mentions = obj["mentions"].toArray();
    if (obj.contains("card") && !obj["card"].toObject().empty()) {
        setCard(std::make_optional<Card>(obj["card"].toObject()));
    }

    for (const auto &m : qAsConst(mentions)) {
        const QJsonObject o = m.toObject();
        m_mentions.push_back("@" + o["acct"].toString());
    }

    if (obj.contains(QStringLiteral("poll")) && !obj[QStringLiteral("poll")].isNull()) {
        m_poll = new Poll(obj[QStringLiteral("poll")].toObject());
    }

    m_attachments_visible = !m_sensitive;
}

Post::~Post()
{
    delete m_poll;
}

void Post::addAttachments(const QJsonArray &attachments)
{
    for (const auto &attachment : attachments) {
        m_attachments.append(Attachment{attachment.toObject()});
    }
}

void Post::addAttachment(const QJsonObject &attachment)
{
    Attachment att(attachment);
    if (att.m_url.isEmpty()) {
        return;
    }
    m_attachments.append(std::move(att));

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

int Post::repliesCount() const
{
    return m_repliesCount;
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

QStringList Post::filters() const
{
    return m_filters;
}

QUrl Post::url() const
{
    return m_url;
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

    obj["spoiler_text"] = m_spoilerText;
    obj["status"] = m_content;
    obj["content_type"] = m_content_type;
    obj["sensitive"] = m_sensitive;
    obj["visibility"] = visibilityToString(m_visibility);

    if (!m_replyTargetId.isEmpty()) {
        obj["in_reply_to_id"] = m_replyTargetId;
    }

    auto media_ids = QJsonArray();
    for (const auto &att : qAsConst(m_attachments)) {
        media_ids.append(att.m_id);
    }

    obj["media_ids"] = media_ids;

    return QJsonDocument(obj);
}

QDateTime Post::publishedAt() const
{
    return m_publishedAt;
}

int Post::favouritesCount() const
{
    return m_favouritesCount;
}

int Post::reblogsCount() const
{
    return m_reblogsCount;
}

static QMap<QString, Notification::Type> str_to_not_type = {
    {"favourite", Notification::Type::Favorite},
    {"follow", Notification::Type::Follow},
    {"mention", Notification::Type::Mention},
    {"reblog", Notification::Type::Repeat},
    {"update", Notification::Type::Update},
    {"poll", Notification::Type::Poll},
};

Notification::Notification(AbstractAccount *account, const QJsonObject &obj, QObject *parent)
    : m_account(account)
{
    const auto accountObj = obj["account"].toObject();
    const auto status = obj["status"].toObject();
    const auto accountId = accountObj["id"].toString();
    const auto type = obj["type"].toString();

    m_post = new Post(m_account, status, parent);
    m_identity = m_account->identityLookup(accountId, accountObj);
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

QString Post::spoilerText() const
{
    return m_spoilerText;
}

void Post::setSpoilerText(const QString &spoilerText)
{
    if (spoilerText == m_spoilerText) {
        return;
    }
    m_spoilerText = spoilerText;
    Q_EMIT spoilerTextChanged();
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

bool Post::sensitive() const
{
    return m_sensitive;
}

void Post::setSensitive(bool sensitive)
{
    if (m_sensitive == sensitive) {
        return;
    }
    m_sensitive = sensitive;
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

bool Post::favourited() const
{
    return m_favourited;
}

void Post::setFavourited(bool favourited)
{
    m_favourited = favourited;
}

bool Post::reblogged() const
{
    return m_reblogged;
}

void Post::setReblogged(bool reblogged)
{
    m_reblogged = reblogged;
}

bool Post::muted() const
{
    return m_muted;
}

void Post::setMuted(bool muted)
{
    m_muted = muted;
}

bool Post::bookmarked() const
{
    return m_bookmarked;
}

void Post::setBookmarked(bool bookmarked)
{
    m_bookmarked = bookmarked;
}

bool Post::pinned() const
{
    return m_pinned;
}

void Post::setPinned(bool pinned)
{
    m_pinned = pinned;
}

bool Post::filtered() const
{
    return m_filtered;
}

QList<Attachment *> Post::attachments() const
{
    return m_attachments;
}

void Post::setAttachmentsVisible(bool attachmentsVisible)
{
    m_attachments_visible = attachmentsVisible;
}

bool Post::attachmentsVisible() const
{
    return m_attachments_visible;
}

bool Post::boosted() const
{
    return m_boosted;
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

std::shared_ptr<Identity> Post::boostIdentity() const
{
    return m_boostIdentity;
}

Poll *Post::poll() const
{
    return m_poll;
}

void Post::setPoll(Poll *poll)
{
    m_poll = poll;
    Q_EMIT pollChanged();
}

QString Post::postId() const
{
    return m_postId;
}

bool Post::isEmpty() const
{
    return m_postId.isEmpty();
}
