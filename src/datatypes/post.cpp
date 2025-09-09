// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "datatypes/post.h"

#include "account/abstractaccount.h"
#include "accountmanager.h"
#include "networkcontroller.h"
#include "tokodon_debug.h"
#include "utils/texthandler.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>

using namespace Qt::Literals::StringLiterals;

static const QMap<Post::Visibility, QString> p_visibilityToString = {
    {Post::Visibility::Public, QStringLiteral("public")},
    {Post::Visibility::Unlisted, QStringLiteral("unlisted")},
    {Post::Visibility::Private, QStringLiteral("private")},
    {Post::Visibility::Direct, QStringLiteral("direct")},
};

static const QMap<QString, Post::Visibility> p_stringToVisibility = {
    {QStringLiteral("public"), Post::Visibility::Public},
    {QStringLiteral("unlisted"), Post::Visibility::Unlisted},
    {QStringLiteral("private"), Post::Visibility::Private},
    {QStringLiteral("direct"), Post::Visibility::Direct},
    {QStringLiteral("local"), Post::Visibility::Local},
};

Post::Post(AbstractAccount *account, QObject *parent)
    : QObject(parent)
    , m_parent(account)
{
    Q_ASSERT(account);
    QString visibilityString = account->identity()->visibility();
    m_visibility = stringToVisibility(visibilityString);
}

Post::Post(AbstractAccount *account, QJsonObject obj, QObject *parent)
    : QObject(parent)
    , m_parent(account)
    , m_visibility(Post::Visibility::Public)
{
    Q_ASSERT(account);
    fromJson(obj);
}

void Post::fromJson(QJsonObject obj)
{
    const auto accountDoc = obj["account"_L1].toObject();
    const auto accountId = accountDoc["id"_L1].toString();

    m_originalPostId = obj["id"_L1].toString();
    const auto reblogObj = obj["reblog"_L1].toObject();

    if (!obj.contains("reblog"_L1) || reblogObj.isEmpty()) {
        m_boosted = false;
        m_authorIdentity = m_parent->identityLookup(accountId, accountDoc);
    } else {
        m_boosted = true;

        const auto reblogAccountDoc = reblogObj["account"_L1].toObject();
        const auto reblogAccountId = reblogAccountDoc["id"_L1].toString();

        m_authorIdentity = m_parent->identityLookup(reblogAccountId, reblogAccountDoc);
        m_boostIdentity = m_parent->identityLookup(accountId, accountDoc);

        obj = reblogObj;
    }

    m_postId = obj["id"_L1].toString();

    m_spoilerText = obj["spoiler_text"_L1].toString();

    processContent(obj);

    // Process all URLs in the body
    auto matchIterator = TextRegex::url.globalMatch(m_content);
    while (matchIterator.hasNext()) {
        const QRegularExpressionMatch match = matchIterator.next();
        // To whittle down the number of requests (which in most cases should be zero) check if the URL could point to a valid post.
        if (TextHandler::isPostUrl(match.captured(0))) {
            // Then request said URL from our server
            m_parent->requestRemoteObject(QUrl(match.captured(0)), this, [this](QNetworkReply *reply) {
                const auto searchResult = QJsonDocument::fromJson(reply->readAll()).object();

                const auto statuses = searchResult[QStringLiteral("statuses")].toArray();

                if (statuses.isEmpty()) {
                    qCDebug(TOKODON_LOG) << "Failed to find any statuses!";
                } else {
                    const auto status = statuses.first().toObject();

                    m_quotedPost = new Post(m_parent, status, this);
                    Q_EMIT quotedPostChanged();
                }
            });

            break;
        }
    }

    m_replyTargetId = obj["in_reply_to_id"_L1].toString();

    if (obj.contains("in_reply_to_account_id"_L1) && obj["in_reply_to_account_id"_L1].isString()) {
        if (m_parent->identityCached(obj["in_reply_to_account_id"_L1].toString())) {
            m_replyIdentity = m_parent->identityLookup(obj["in_reply_to_account_id"_L1].toString(), {});
        } else {
            const auto accountId = obj["in_reply_to_account_id"_L1].toString();
            m_parent->get(m_parent->apiUrl(QStringLiteral("/api/v1/accounts/%1").arg(accountId)), true, this, [this, accountId](QNetworkReply *reply) {
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

            m_replyIdentity = m_parent->identityLookup(doc["account"_L1].toObject()["id"_L1].toString(), doc["account"_L1].toObject());
            Q_EMIT replyIdentityChanged();
        });
    }

    m_url = QUrl(obj["url"_L1].toString());

    m_favouritesCount = obj["favourites_count"_L1].toInt();
    m_reblogsCount = obj["reblogs_count"_L1].toInt();
    m_repliesCount = obj["replies_count"_L1].toInt();

    m_favourited = obj["favourited"_L1].toBool();
    m_reblogged = obj["reblogged"_L1].toBool();
    m_bookmarked = obj["bookmarked"_L1].toBool();
    m_pinned = obj["pinned"_L1].toBool();
    m_muted = obj["muted"_L1].toBool();

    m_filters.clear();

    const auto filters = obj["filtered"_L1].toArray();
    for (const auto &filter : filters) {
        const auto filterContext = filter.toObject();
        const auto filterObj = filterContext["filter"_L1].toObject();
        m_filters << filterObj["title"_L1].toString();

        const auto filterAction = filterObj["filter_action"_L1];
        if (filterAction == "warn"_L1) {
            m_filtered = true;
        } else if (filterAction == "hide"_L1) {
            m_hidden = true;
        }
    }

    m_sensitive = obj["sensitive"_L1].toBool();
    m_visibility = stringToVisibility(obj["visibility"_L1].toString());
    m_language = obj["language"_L1].toString();

    m_publishedAt = QDateTime::fromString(obj["created_at"_L1].toString(), Qt::ISODate).toLocalTime();

    if (!obj["edited_at"_L1].isNull()) {
        m_editedAt = QDateTime::fromString(obj["edited_at"_L1].toString(), Qt::ISODate).toLocalTime();
    }

    m_attachments.clear();
    addAttachments(obj["media_attachments"_L1].toArray());
    const QJsonArray mentions = obj["mentions"_L1].toArray();
    if (obj.contains("card"_L1) && !obj["card"_L1].toObject().empty()) {
        setCard(std::make_optional<Card>(m_parent, obj["card"_L1].toObject()));
    }

    if (obj.contains("application"_L1) && !obj["application"_L1].toObject().empty()) {
        setApplication(std::make_optional<Application>(obj["application"_L1].toObject()));
    }

    m_mentions.clear();
    for (const auto &m : std::as_const(mentions)) {
        const QJsonObject o = m.toObject();
        m_mentions.push_back(QStringLiteral("@") + o["acct"_L1].toString());
    }

    if (obj.contains(QStringLiteral("poll")) && !obj[QStringLiteral("poll")].isNull()) {
        m_poll = std::make_unique<Poll>(obj[QStringLiteral("poll")].toObject());
    }
}

QString Post::postId() const
{
    return m_postId;
}

QString Post::originalPostId() const
{
    return m_originalPostId;
}

QDateTime Post::publishedAt() const
{
    return m_publishedAt;
}

QString Post::relativeTime() const
{
    return TextHandler::getRelativeDateTime(publishedAt());
}

QString Post::absoluteTime() const
{
    return QLocale::system().toString(publishedAt(), QLocale::LongFormat);
}

std::shared_ptr<Identity> Post::authorIdentity() const
{
    return m_authorIdentity;
}

QString Post::content() const
{
    return m_content;
}

bool Post::hasContent() const
{
    return m_hasContent;
}

Post::Visibility Post::visibility() const
{
    return m_visibility;
}

bool Post::sensitive() const
{
    return m_sensitive;
}

QString Post::spoilerText() const
{
    return m_spoilerText;
}

QList<Attachment *> Post::attachments() const
{
    return m_attachments;
}

std::optional<Application> Post::application() const
{
    return m_application;
}

QStringList Post::mentions() const
{
    return m_mentions;
}

QVector<QString> Post::standaloneTags() const
{
    return m_standaloneTags;
}

int Post::repliesCount() const
{
    return m_repliesCount;
}

int Post::favouritesCount() const
{
    return m_favouritesCount;
}

int Post::reblogsCount() const
{
    return m_reblogsCount;
}

QUrl Post::url() const
{
    return m_url;
}

QString Post::inReplyTo() const
{
    return m_replyTargetId;
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

std::optional<Card> Post::card() const
{
    return m_card;
}

QString Post::language() const
{
    return m_language;
}

bool Post::wasEdited() const
{
    return m_editedAt.isValid();
}

QString Post::editedAt() const
{
    return QLocale::system().toString(m_editedAt, QLocale::ShortFormat);
}

bool Post::boosted() const
{
    return m_boosted;
}

std::shared_ptr<Identity> Post::boostIdentity() const
{
    return m_boostIdentity;
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

bool Post::bookmarked() const
{
    return m_bookmarked;
}

void Post::setBookmarked(bool bookmarked)
{
    m_bookmarked = bookmarked;
}

bool Post::muted() const
{
    return m_muted;
}

void Post::setMuted(bool muted)
{
    m_muted = muted;
}

QStringList Post::filters() const
{
    return m_filters;
}

bool Post::hidden() const
{
    return m_hidden;
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

void Post::addAttachments(const QJsonArray &attachments)
{
    for (const auto &attachment : attachments) {
        m_attachments.append(new Attachment{attachment.toObject(), this});
    }
}

QString Post::visibilityToString(Post::Visibility visibility)
{
    return p_visibilityToString[visibility];
}

Post::Visibility Post::stringToVisibility(const QString &visibility)
{
    return p_stringToVisibility[visibility];
}

QString Post::type() const
{
    return QStringLiteral("post");
}

Identity *Post::getAuthorIdentity() const
{
    return authorIdentity().get();
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

Post *Post::quotedPost() const
{
    return m_quotedPost;
}

void Post::setApplication(std::optional<Application> application)
{
    m_application = application;
}

void Post::processContent(const QJsonObject &obj)
{
    const QString originalHtml = obj["content"_L1].toString();

    // First, replace custom emojis with their HTML representations
    const auto emojis = CustomEmoji::parseCustomEmojis(obj["emojis"_L1].toArray());
    QString processedHtml = TextHandler::replaceCustomEmojis(emojis, originalHtml);

    // Then turn hashtags into proper links, so they link inside Tokodon
    const auto tags = obj["tags"_L1].toArray();
    const QString baseUrl = m_authorIdentity->url().toDisplayString(QUrl::RemovePath);

    for (const auto &tag : tags) {
        const auto tagObj = tag.toObject();

        // The "url" field in the tag object is for our own instance,
        // but the url for the tag in the HTML we're given is for their instance. Hence, the odd search & replace done here.
        const QList<QString> tagFormats = {
            QStringLiteral("tags"), // Mastodon
            QStringLiteral("tag") // Akkoma/Pleroma
        };

        for (const QString &tagFormat : tagFormats) {
            const QString tagName = tagObj["name"_L1].toString();

            processedHtml.replace(QStringLiteral("%1/%2/%3").arg(baseUrl, tagFormat, tagName), QStringLiteral("hashtag:/%1").arg(tagName), Qt::CaseInsensitive);
        }
    }

    // Do the same for mentions
    const auto mentions = obj["mentions"_L1].toArray();

    // Go through all link tags
    auto matchIterator = TextRegex::linkTags.globalMatch(processedHtml);
    while (matchIterator.hasNext()) {
        const QRegularExpressionMatch match = matchIterator.next();
        // Check if it's actually a mention, to prevent it from overwriting post URLs for example
        if (match.captured(0).contains(QStringLiteral("class=\"u-url mention\""))) {
            for (const auto &mention : mentions) {
                // Replace if the mention URL matches with an internal Tokodon account URI
                if (mention["url"_L1].toString() == match.captured(1)) {
                    processedHtml.replace(match.capturedStart(1), match.capturedLength(1), QStringLiteral("account:/") + mention["id"_L1].toString());
                    break;
                }
            }
        }
    }

    // Remove the standalone tags from the main content
    auto [standaloneContent, standaloneTags] = TextHandler::removeStandaloneTags(processedHtml);
    m_standaloneTags = standaloneTags;

    m_hasContent = !standaloneContent.isEmpty();
    m_content = standaloneContent;
}

bool Post::hasPoll() const
{
    return m_poll.operator bool();
}

#include "moc_post.cpp"
