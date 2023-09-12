// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-License-Identifier: GPL-3.0-only

#include <KLocalizedString>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QtMath>
#include <qurl.h>

#include "account/abstractaccount.h"
#include "poll.h"
#include "post.h"
#include "utils/utils.h"

static QMap<QString, Attachment::AttachmentType> stringToAttachmentType = {
    {"image", Attachment::AttachmentType::Image},
    {"gifv", Attachment::AttachmentType::GifV},
    {"video", Attachment::AttachmentType::Video},
    {"unknown", Attachment::AttachmentType::Unknown},
};

Attachment::Attachment(QObject *parent)
    : QObject(parent)
{
}

Attachment::Attachment(const QJsonObject &obj, QObject *parent)
    : QObject(parent)
{
    fromJson(obj);
}

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
    m_sourceHeight = obj["meta"].toObject()["original"].toObject()["height"].toInt();
    m_sourceWidth = obj["meta"].toObject()["original"].toObject()["width"].toInt();

    // determine type if we can
    const auto type = obj["type"].toString();
    if (stringToAttachmentType.contains(type)) {
        m_type = stringToAttachmentType[type];
    }

    if (obj.contains("meta") && obj["meta"].toObject().contains("focus")) {
        m_focusX = obj["meta"].toObject()["focus"].toObject()["x"].toDouble();
        m_focusY = obj["meta"].toObject()["focus"].toObject()["y"].toDouble();
    }
}

QString Post::type() const
{
    return "post";
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

int Attachment::isVideo() const
{
    if (m_type == AttachmentType::GifV || m_type == AttachmentType::Video) {
        return 1;
    }

    return 0;
}

QString Attachment::tempSource() const
{
    return QString("image://blurhash/%1").arg(m_blurhash);
}

double Attachment::focusX() const
{
    return m_focusX;
}

void Attachment::setFocusX(double value)
{
    if (value != m_focusX) {
        m_focusX = value;
        Q_EMIT focusXChanged();
    }
}

double Attachment::focusY() const
{
    return m_focusY;
}

void Attachment::setFocusY(double value)
{
    if (value != m_focusY) {
        m_focusY = value;
        Q_EMIT focusYChanged();
    }
}

Post::Post(AbstractAccount *account, QObject *parent)
    : QObject(parent)
    , m_parent(account)
    , m_attachmentList(this, &m_attachments)
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

    const auto mentions = obj["mentions"].toArray();

    for (const auto &mention : mentions) {
        const auto mentionObj = mention.toObject();
        content = content.replace(mentionObj["url"].toString(), QStringLiteral("account:/") + mentionObj["id"].toString(), Qt::CaseInsensitive);
    }

    return content;
}

Post::Post(AbstractAccount *account, QJsonObject obj, QObject *parent)
    : QObject(parent)
    , m_parent(account)
    , m_attachmentList(this, &m_attachments)
    , m_visibility(Post::Visibility::Public)
{
    fromJson(obj);
}

Post *Notification::createPost(AbstractAccount *account, const QJsonObject &obj, QObject *parent)
{
    if (!obj.empty()) {
        return new Post(account, obj, parent);
    }

    return nullptr;
}

void Post::fromJson(QJsonObject obj)
{
    const auto accountDoc = obj["account"].toObject();
    const auto accountId = accountDoc["id"].toString();

    m_originalPostId = obj["id"].toString();
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

    m_postId = obj["id"].toString();

    m_spoilerText = obj["spoiler_text"].toString();
    m_content = computeContent(obj, m_authorIdentity);

    m_replyTargetId = obj["in_reply_to_id"].toString();

    if (obj.contains("in_reply_to_account_id") && obj["in_reply_to_account_id"].isString()) {
        if (m_parent->identityCached(obj["in_reply_to_account_id"].toString())) {
            m_replyIdentity = m_parent->identityLookup(obj["in_reply_to_account_id"].toString(), {});
        } else {
            const auto accountId = obj["in_reply_to_account_id"].toString();
            QUrl uriAccount(m_parent->instanceUri());
            uriAccount.setPath(QStringLiteral("/api/v1/accounts/%1").arg(accountId));

            m_parent->get(uriAccount, true, this, [this, accountId](QNetworkReply *reply) {
                const auto data = reply->readAll();
                const auto doc = QJsonDocument::fromJson(data);

                m_replyIdentity = m_parent->identityLookup(accountId, doc.object());
                Q_EMIT replyIdentityChanged();
            });
        }
    } else if (!m_replyTargetId.isEmpty()) {
        // Fallback to getting the account id from the status, which is weird but this sometimes has to happen.
        m_parent->get(m_parent->apiUrl(QStringLiteral("/api/v1/statuses/%1").arg(m_replyTargetId)), true, this, [this](QNetworkReply *reply) {
            const auto data = reply->readAll();
            const auto doc = QJsonDocument::fromJson(data);

            m_replyIdentity = m_parent->identityLookup(doc["account"].toObject()["id"].toString(), doc["account"].toObject());
            Q_EMIT replyIdentityChanged();
        });
    }

    m_url = QUrl(obj["url"].toString());

    m_favouritesCount = obj["favourites_count"].toInt();
    m_reblogsCount = obj["reblogs_count"].toInt();
    m_repliesCount = obj["replies_count"].toInt();

    m_favourited = obj["favourited"].toBool();
    m_reblogged = obj["reblogged"].toBool();
    m_bookmarked = obj["bookmarked"].toBool();
    m_pinned = obj["pinned"].toBool();
    m_muted = obj["muted"].toBool();

    m_filters.clear();

    const auto filters = obj["filtered"].toArray();
    for (const auto &filter : filters) {
        const auto filterContext = filter.toObject();
        const auto filterObj = filterContext["filter"].toObject();
        m_filters << filterObj["title"].toString();

        const auto filterAction = filterObj["filter_action"];
        if (filterAction == "warn") {
            m_filtered = true;
        } else if (filterAction == "hide") {
            m_hidden = true;
        }
    }

    m_sensitive = obj["sensitive"].toBool();
    m_visibility = stringToVisibility(obj["visibility"].toString());
    m_language = obj["language"].toString();

    m_publishedAt = QDateTime::fromString(obj["created_at"].toString(), Qt::ISODate).toLocalTime();

    m_attachments.clear();
    addAttachments(obj["media_attachments"].toArray());
    const QJsonArray mentions = obj["mentions"].toArray();
    if (obj.contains("card") && !obj["card"].toObject().empty()) {
        setCard(std::make_optional<Card>(obj["card"].toObject()));
    }

    if (obj.contains("application") && !obj["application"].toObject().empty()) {
        setApplication(std::make_optional<Application>(obj["application"].toObject()));
    }

    m_mentions.clear();
    for (const auto &m : qAsConst(mentions)) {
        const QJsonObject o = m.toObject();
        m_mentions.push_back("@" + o["acct"].toString());
    }

    if (obj.contains(QStringLiteral("poll")) && !obj[QStringLiteral("poll")].isNull()) {
        m_poll = std::make_unique<Poll>(obj[QStringLiteral("poll")].toObject());
    }

    m_attachments_visible = !m_sensitive;
}

void Post::addAttachments(const QJsonArray &attachments)
{
    for (const auto &attachment : attachments) {
        m_attachments.append(new Attachment{attachment.toObject(), this});
    }
}

void Post::addAttachment(const QJsonObject &attachment)
{
    auto att = new Attachment{attachment, this};
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

QDateTime Post::publishedAt() const
{
    return m_publishedAt;
}

QString Post::relativeTime() const
{
    const auto current = QDateTime::currentDateTime();
    const auto publishingDate = publishedAt();
    const auto secsTo = publishingDate.secsTo(current);
    const auto daysTo = publishingDate.daysTo(current);
    if (secsTo < 0) {
        return i18n("in the future");
    } else if (secsTo < 60) {
        return i18n("%1s", qCeil(secsTo));
    } else if (secsTo < 60 * 60) {
        return i18n("%1m", qCeil(secsTo / 60));
    } else if (secsTo < 60 * 60 * 24) {
        return i18n("%1h", qCeil(secsTo / (60 * 60)));
    } else if (daysTo < 7) {
        return i18n("%1d", qCeil(daysTo));
    } else if (daysTo < 365) {
        const auto weeksTo = qCeil(daysTo / 7);
        if (weeksTo < 5) {
            return i18np("1 week ago", "%1 weeks ago", weeksTo);
        } else {
            const auto monthsTo = qCeil(daysTo / 30);
            return i18np("1 month ago", "%1 months ago", monthsTo);
        }
    } else {
        const auto yearsTo = qCeil(daysTo / 365);
        return i18np("1 year ago", "%1 years ago", yearsTo);
    }
}

QString Post::absoluteTime() const
{
    return QLocale::system().toString(publishedAt(), QLocale::LongFormat);
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
    {"status", Notification::Type::Status},
};

Notification::Notification(AbstractAccount *account, const QJsonObject &obj, QObject *parent)
    : m_account(account)
{
    const auto accountObj = obj["account"].toObject();
    const auto status = obj["status"].toObject();
    const auto accountId = accountObj["id"].toString();
    const auto type = obj["type"].toString();

    m_post = createPost(m_account, status, parent);
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

QString Post::language() const
{
    return m_language;
}

void Post::setLanguage(const QString &language)
{
    if (language == m_language) {
        return;
    }
    m_language = language;
    Q_EMIT languageChanged();
}

std::optional<Card> Post::card() const
{
    return m_card;
}

Card *Post::getCard() const
{
    if (m_card.has_value()) {
        return const_cast<Card *>(&m_card.value());
    } else {
        return nullptr;
    }
}

void Post::setCard(std::optional<Card> card)
{
    m_card = card;
}

std::optional<Application> Post::application() const
{
    return m_application;
}

void Post::setApplication(std::optional<Application> application)
{
    m_application = application;
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

QQmlListProperty<Attachment> Post::attachmentList() const
{
    return m_attachmentList;
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

Application::Application(QJsonObject application)
    : m_application(application)
{
}

QString Application::name() const
{
    return m_application[QLatin1String("name")].toString();
}

QUrl Application::website() const
{
    return QUrl::fromUserInput(m_application[QLatin1String("website")].toString());
}

Identity *Post::getAuthorIdentity() const
{
    return authorIdentity().get();
}

std::shared_ptr<Identity> Post::authorIdentity() const
{
    return m_authorIdentity;
}

std::shared_ptr<Identity> Post::boostIdentity() const
{
    return m_boostIdentity;
}

std::shared_ptr<Identity> Post::replyIdentity() const
{
    return m_replyIdentity;
}

Poll *Post::poll() const
{
    return m_poll.get();
}

void Post::setPollJson(const QJsonObject &object)
{
    m_poll = std::make_unique<Poll>(object);
    Q_EMIT pollChanged();
}

QString Post::postId() const
{
    return m_postId;
}

QString Post::originalPostId() const
{
    return m_originalPostId;
}

bool Post::isEmpty() const
{
    return m_postId.isEmpty();
}
