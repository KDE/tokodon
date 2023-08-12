// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "abstractaccount.h"

#include "accountmanager.h"
#include "preferences.h"
#include "relationship.h"
#include "utils/messagefiltercontainer.h"
#include "tokodon_debug.h"
#include <KLocalizedString>
#include <QFile>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QUrlQuery>

AbstractAccount::AbstractAccount(QObject *parent, const QString &instanceUri)
    : QObject(parent)
    , m_instance_uri(instanceUri)
    // default to 500, instances which support more signal it
    , m_maxPostLength(500)
    , m_maxPollOptions(4)
    , m_charactersReservedPerUrl(23)
    , m_identity(std::make_shared<Identity>())
    , m_allowedContentTypes(AllowedContentType::PlainText)
{
}

AbstractAccount::AbstractAccount(QObject *parent)
    : QObject(parent)
    // default to 500, instances which support more signal it
    , m_maxPostLength(500)
    , m_maxPollOptions(4)
    , m_charactersReservedPerUrl(23)
    , m_identity(std::make_shared<Identity>())
    , m_allowedContentTypes(AllowedContentType::PlainText)
{
}

Preferences *AbstractAccount::preferences() const
{
    return m_preferences;
}

QString AbstractAccount::username() const
{
    return m_name;
}

void AbstractAccount::setUsername(const QString &username)
{
    if (m_name == username) {
        return;
    }
    m_name = username;
    Q_EMIT usernameChanged();
}

size_t AbstractAccount::maxPostLength() const
{
    return m_maxPostLength;
}

size_t AbstractAccount::maxPollOptions() const
{
    return m_maxPollOptions;
}

size_t AbstractAccount::charactersReservedPerUrl() const
{
    return m_charactersReservedPerUrl;
}

QString AbstractAccount::instanceName() const
{
    return m_instance_name;
}

bool AbstractAccount::haveToken() const
{
    return !m_token.isEmpty();
}

bool AbstractAccount::hasName() const
{
    return !m_name.isEmpty();
}

bool AbstractAccount::hasInstanceUrl() const
{
    return !m_instance_uri.isEmpty();
}

QUrl AbstractAccount::apiUrl(const QString &path) const
{
    const auto instanceUrl = QUrl::fromUserInput(m_instance_uri);

    auto url = QUrl::fromUserInput(m_instance_uri);
    url.setScheme("https");
    url.setPath(instanceUrl.path() + path);

    return url;
}

void AbstractAccount::registerApplication(const QString &appName, const QString &website)
{
    // clear any previous bearer token credentials
    m_token = QString();

    // register
    const QUrl regUrl = apiUrl("/api/v1/apps");
    const QJsonObject obj = {
        {"client_name", appName},
        {"redirect_uris", "tokodon://oauth"},
        {"scopes", "read write follow admin:read admin:write"},
        {"website", website},
    };
    const QJsonDocument doc(obj);

    post(regUrl, doc, false, this, [=](QNetworkReply *reply) {
        if (!reply->isFinished()) {
            qCDebug(TOKODON_LOG) << "not finished";
            return;
        }

        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        m_client_id = doc.object()["client_id"].toString();
        m_client_secret = doc.object()["client_secret"].toString();

        s_messageFilter->insert(m_client_secret, "CLIENT_SECRET");

        if (isRegistered()) {
            Q_EMIT registered();
        }
    });
}

Identity *AbstractAccount::identity()
{
    return m_identity.get();
}

std::shared_ptr<Identity> AbstractAccount::identityLookup(const QString &accountId, const QJsonObject &doc)
{
    if (m_identity && m_identity->id() == accountId) {
        return m_identity;
    }
    auto id = m_identityCache[accountId];
    if (id && id->id() == accountId) {
        return id;
    }

    id = std::make_shared<Identity>();
    id->reparentIdentity(this);
    id->fromSourceData(doc);
    m_identityCache[accountId] = id;

    return m_identityCache[accountId];
}

std::shared_ptr<AdminAccountInfo> AbstractAccount::adminIdentityLookup(const QString &accountId, const QJsonObject &doc)
{
    if (m_adminIdentity && m_adminIdentity->userLevelIdentity()->id() == accountId) {
        return m_adminIdentity;
    }
    auto id = m_adminIdentityCache[accountId];
    if (id && id->userLevelIdentity()->id() == accountId) {
        return id;
    }

    id = std::make_shared<AdminAccountInfo>();
    id->reparentAdminAccountInfo(this);
    id->fromSourceData(doc);
    m_adminIdentityCache[accountId] = id;

    return m_adminIdentityCache[accountId];
}

bool AbstractAccount::identityCached(const QString &accountId) const
{
    if (m_identity && m_identity->id() == accountId) {
        return true;
    }
    auto id = m_identityCache[accountId];
    return id && id->id() == accountId;
}

QUrlQuery AbstractAccount::buildOAuthQuery() const
{
    return QUrlQuery{{"client_id", m_client_id}};
}

QUrl AbstractAccount::getAuthorizeUrl() const
{
    QUrl url = apiUrl("/oauth/authorize");
    QUrlQuery q = buildOAuthQuery();

    q.addQueryItem("redirect_uri", "tokodon://oauth");
    q.addQueryItem("response_type", "code");
    q.addQueryItem("scope", "read write follow admin:read admin:write");

    url.setQuery(q);

    return url;
}

QUrl AbstractAccount::getTokenUrl() const
{
    return apiUrl("/oauth/token");
}

void AbstractAccount::setInstanceUri(const QString &instance_uri)
{
    // instance URI changed, get new credentials
    QUrl instance_url = QUrl::fromUserInput(instance_uri);
    instance_url.setScheme("https"); // getting token from http is not supported

    m_instance_uri = instance_url.toString();
    registerApplication("Tokodon", "https://apps.kde.org/tokodon");
}

QString AbstractAccount::instanceUri() const
{
    return m_instance_uri;
}

void AbstractAccount::setToken(const QString &authcode)
{
    const QUrl tokenUrl = getTokenUrl();
    QUrlQuery q = buildOAuthQuery();

    q.addQueryItem("client_secret", m_client_secret);
    q.addQueryItem("redirect_uri", "tokodon://oauth");
    q.addQueryItem("grant_type", "authorization_code");
    q.addQueryItem("code", authcode);

    post(tokenUrl, q, false, this, [=](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);

        m_token = doc.object()["access_token"].toString();
        s_messageFilter->insert(m_token, "ACCESS_TOKEN");
        AccountManager::instance().addAccount(this);
        AccountManager::instance().selectAccount(this, true);
        validateToken();
    });
}

void AbstractAccount::mutatePost(Post *p, const QString &verb, bool deliver_home)
{
    const QUrl mutation_url = apiUrl(QString("/api/v1/statuses/%1/%2").arg(p->postId(), verb));
    const QJsonDocument doc;

    post(mutation_url, doc, true, this, [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (deliver_home) {
            QList<Post *> posts;
            auto obj = doc.object();

            auto p = new Post(this, obj, this);
            posts.push_back(p);

            Q_EMIT fetchedTimeline("home", posts);
        }
    });
}

void AbstractAccount::favorite(Post *p)
{
    mutatePost(p, "favourite");
}

void AbstractAccount::unfavorite(Post *p)
{
    mutatePost(p, "unfavourite");
}

void AbstractAccount::repeat(Post *p)
{
    mutatePost(p, "reblog", true);
}

void AbstractAccount::unrepeat(Post *p)
{
    mutatePost(p, "unreblog");
}

void AbstractAccount::bookmark(Post *p)
{
    mutatePost(p, "bookmark", true);
}

void AbstractAccount::unbookmark(Post *p)
{
    mutatePost(p, "unbookmark");
}

void AbstractAccount::pin(Post *p)
{
    mutatePost(p, "pin", true);
}

void AbstractAccount::unpin(Post *p)
{
    mutatePost(p, "unpin");
}

// It seemed clearer to keep this logic separate from the general instance metadata collection, on the off chance
// that it might need to be extended later on.
static AbstractAccount::AllowedContentType parseVersion(const QString &instanceVer)
{
    using ContentType = AbstractAccount::AllowedContentType;

    unsigned int result = ContentType::PlainText;
    if (instanceVer.contains("glitch")) {
        result |= ContentType::Markdown | ContentType::Html;
    }

    return static_cast<ContentType>(result);
}

static QMap<QString, AbstractAccount::AllowedContentType> stringToContentType = {
    {"text/plain", AbstractAccount::AllowedContentType::PlainText},
    {"text/bbcode", AbstractAccount::AllowedContentType::BBCode},
    {"text/html", AbstractAccount::AllowedContentType::Html},
    {"text/markdown", AbstractAccount::AllowedContentType::Markdown},
};

static AbstractAccount::AllowedContentType parsePleromaInfo(const QJsonDocument &doc)
{
    using ContentType = AbstractAccount::AllowedContentType;
    unsigned int result = ContentType::PlainText;

    auto obj = doc.object();
    if (obj.contains("metadata")) {
        auto metadata = obj["metadata"].toObject();
        if (!metadata.contains("postFormats"))
            return static_cast<ContentType>(result);

        auto formats = metadata["postFormats"].toArray();

        for (auto c : formats) {
            auto fmt = c.toString();

            if (!stringToContentType.contains(fmt))
                continue;

            result |= (unsigned int)stringToContentType[fmt];
        }
    }

    return static_cast<ContentType>(result);
}

void AbstractAccount::fetchInstanceMetadata()
{
    QUrl instance_url = apiUrl("/api/v1/instance");
    QUrl pleroma_info = apiUrl("/nodeinfo/2.1.json");

    get(instance_url, false, this, [=](QNetworkReply *reply) {
        if (200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute))
            return;

        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isObject())
            return;

        const auto obj = doc.object();

        if (obj.contains("configuration")) {
            const auto configObj = obj["configuration"].toObject();

            if (configObj.contains("statuses")) {
                const auto statusConfigObj = configObj["statuses"].toObject();
                m_maxPostLength = statusConfigObj["max_characters"].toInt();
                m_charactersReservedPerUrl = statusConfigObj["characters_reserved_per_url"].toInt();
            }
        }

        // One can only hope that there will always be a version attached
        if (obj.contains("version")) {
            m_allowedContentTypes = parseVersion(obj["version"].toString());
        }

        // Pleroma/Akkoma may report maximum post characters here, instead
        if (obj.contains("max_toot_chars")) {
            m_maxPostLength = obj["max_toot_chars"].toInt();
        }

        // Pleroma/Akkoma can report higher poll limits
        if (obj.contains("poll_limits")) {
            m_maxPollOptions = obj["poll_limits"].toObject()["max_options"].toInt();
        }

        m_instance_name = obj["title"].toString();
        Q_EMIT fetchedInstanceMetadata();
    });
    m_instance_name = QString("social");
    Q_EMIT fetchedInstanceMetadata();

    get(pleroma_info, false, this, [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        m_allowedContentTypes = parsePleromaInfo(doc);
        Q_EMIT fetchedInstanceMetadata();
    });

    fetchCustomEmojis();
}

void AbstractAccount::invalidate()
{
    Q_EMIT invalidated();
}

void AbstractAccount::invalidatePost(Post *p)
{
    Q_EMIT invalidatedPost(p);
}

QUrl AbstractAccount::streamingUrl(const QString &stream)
{
    QUrl url = apiUrl("/api/v1/streaming");
    url.setQuery(QUrlQuery{
        {"access_token", m_token},
        {"stream", stream},
    });
    url.setScheme("wss");

    return url;
}

void AbstractAccount::handleNotification(const QJsonDocument &doc)
{
    const auto obj = doc.object();
    std::shared_ptr<Notification> n = std::make_shared<Notification>(this, obj);

    Q_EMIT notification(n);
}

void AbstractAccount::executeAction(Identity *identity, AccountAction accountAction, const QJsonObject &extraArguments)
{
    const QHash<AccountAction, QString> accountActionMap = {
        {AccountAction::Follow, QStringLiteral("/follow")},
        {AccountAction::Unfollow, QStringLiteral("/unfollow")},
        {AccountAction::Block, QStringLiteral("/block")},
        {AccountAction::Unblock, QStringLiteral("/unblock")},
        {AccountAction::Mute, QStringLiteral("/mute")},
        {AccountAction::Unmute, QStringLiteral("/unmute")},
        {AccountAction::Feature, QStringLiteral("/pin")},
        {AccountAction::Unfeature, QStringLiteral("/unpin")},
        {AccountAction::Note, QStringLiteral("/note")},
    };

    const auto apiCall = accountActionMap[accountAction];

    const auto accountId = identity->id();
    const QString accountApiUrl = QStringLiteral("/api/v1/accounts/") + accountId + apiCall;
    const QJsonDocument doc(extraArguments);

    post(apiUrl(accountApiUrl), doc, true, this, [=](QNetworkReply *reply) {
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto jsonObj = doc.object();

        // Check if the request failed due to one account blocking the other
        if (!jsonObj.value("error").isUndefined()) {
            const QHash<AccountAction, QString> accountActionMap = {
                {AccountAction::Follow, i18n("Could not follow account")},
                {AccountAction::Unfollow, i18n("Could not unfollow account")},
                {AccountAction::Block, i18n("Could not block account")},
                {AccountAction::Unblock, i18n("Could not unblock account")},
                {AccountAction::Mute, i18n("Could not mute account")},
                {AccountAction::Unmute, i18n("Could not unmute account")},
                {AccountAction::Feature, i18n("Could not feature account")},
                {AccountAction::Unfeature, i18n("Could not unfeature account")},
                {AccountAction::Note, i18n("Could not edit note about an account")},
            };
            const auto errorMessage = accountActionMap[accountAction];
            Q_EMIT errorOccured(errorMessage);
            return;
        }
        // If returned json obj is not an error, it's a relationship status.
        // Returned relationship should have a value of true
        // under either the "following" or "requested" keys.
        auto relationship = identity->relationship();
        relationship->updateFromJson(jsonObj);

        Q_EMIT identity->relationshipChanged();
    });
}

void AbstractAccount::followAccount(Identity *identity, bool reblogs, bool notify)
{
    executeAction(identity,
                  AccountAction::Follow,
                  {
                      {"reblogs", reblogs},
                      {"notify", notify},
                  });
}

void AbstractAccount::unfollowAccount(Identity *identity)
{
    executeAction(identity, AccountAction::Unfollow);
}

void AbstractAccount::blockAccount(Identity *identity)
{
    executeAction(identity, AccountAction::Block);
}

void AbstractAccount::unblockAccount(Identity *identity)
{
    executeAction(identity, AccountAction::Unblock);
}

void AbstractAccount::muteAccount(Identity *identity, bool notifications, int duration)
{
    executeAction(identity, AccountAction::Mute, {{"notifcations", notifications}, {"duration", duration}});
}

void AbstractAccount::unmuteAccount(Identity *identity)
{
    executeAction(identity, AccountAction::Unmute);
}

void AbstractAccount::featureAccount(Identity *identity)
{
    executeAction(identity, AccountAction::Feature);
}

void AbstractAccount::unfeatureAccount(Identity *identity)
{
    executeAction(identity, AccountAction::Unfeature);
}

void AbstractAccount::addNote(Identity *identity, const QString &note)
{
    if (note.isEmpty()) {
        executeAction(identity, AccountAction::Note);
    } else {
        executeAction(identity, AccountAction::Note, {{"comment", note}});
    }
}

bool AbstractAccount::isRegistered() const
{
    return !m_client_id.isEmpty() && !m_client_secret.isEmpty();
}

QString AbstractAccount::settingsGroupName() const
{
    return AccountManager::settingsGroupName(m_name, m_instance_uri);
}

QString AbstractAccount::clientSecretKey() const
{
    return AccountManager::clientSecretKey(settingsGroupName());
}

QString AbstractAccount::accessTokenKey() const
{
    return AccountManager::accessTokenKey(settingsGroupName());
}

void AbstractAccount::fetchCustomEmojis()
{
    m_customEmojis.clear();

    get(apiUrl("/api/v1/custom_emojis"), false, this, [=](QNetworkReply *reply) {
        if (200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute))
            return;

        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isArray())
            return;

        const auto array = doc.array();

        for (auto emojiObj : array) {
            if (!emojiObj.isObject()) {
                continue;
            }

            CustomEmoji customEmoji{};
            customEmoji.shortcode = emojiObj[QStringLiteral("shortcode")].toString();
            customEmoji.url = emojiObj[QStringLiteral("url")].toString();

            m_customEmojis.push_back(customEmoji);
        }

        Q_EMIT fetchedCustomEmojis();
    });
}

QList<CustomEmoji> AbstractAccount::customEmojis() const
{
    return m_customEmojis;
}
