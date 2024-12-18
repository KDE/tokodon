// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "account/abstractaccount.h"

#include "account/accountmanager.h"
#include "account/relationship.h"
#include "network/networkcontroller.h"
#include "utils/messagefiltercontainer.h"
#include "utils/navigation.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>

using namespace Qt::Literals::StringLiterals;

AbstractAccount::AbstractAccount(const QString &instanceUri, QObject *parent)
    : QObject(parent)
    , m_instance_uri(instanceUri)
    // default to 500, instances which support more signal it
    , m_maxPostLength(500)
    , m_maxPollOptions(4)
    , m_supportsLocalVisibility(false)
    , m_charactersReservedPerUrl(23)
    , m_identity(std::make_shared<Identity>())
    , m_preferences(new Preferences(this))
    , m_notificationFilteringPolicy(new NotificationFilteringPolicy(this))
{
    // Test code uses a blank instance URI
    if (!AccountManager::instance().testMode()) {
        Q_ASSERT(!instanceUri.isEmpty());
    }
}

AccountConfig *AbstractAccount::config()
{
    if (!m_config) {
        // do not write to settings if we do not have complete information yet,
        // or else it writes malformed and possibly duplicate accounts to settings.
        if (!m_name.isEmpty() && !m_instance_uri.isEmpty()) {
            m_config = new AccountConfig{settingsGroupName(), this};
        }
    }
    return m_config;
}

Preferences *AbstractAccount::preferences() const
{
    return m_preferences;
}

NotificationFilteringPolicy *AbstractAccount::notificationFilteringPolicy() const
{
    return m_notificationFilteringPolicy;
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

bool AbstractAccount::supportsLocalVisibility() const
{
    return m_supportsLocalVisibility;
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

QUrl AbstractAccount::apiUrl(const QString &path) const
{
    QUrl url = QUrl::fromUserInput(m_instance_uri);
    url.setScheme(QStringLiteral("https"));
    url.setPath(path);

    return url;
}

void AbstractAccount::registerApplication(const QString &appName, const QString &website, const QString &additionalScopes, const bool useAuthCode)
{
    // clear any previous bearer token credentials
    m_token.clear();

    QString ourAdditionalScopes;
#ifdef HAVE_KUNIFIEDPUSH
    ourAdditionalScopes = QStringLiteral("push ");
#endif

    // Store for future usage (e.g. authorizeUrl)
    m_additionalScopes = ourAdditionalScopes + additionalScopes;

    m_redirectUri = useAuthCode ? QStringLiteral("urn:ietf:wg:oauth:2.0:oob") : QStringLiteral("tokodon://oauth");

    // register
    const QUrl regUrl = apiUrl(QStringLiteral("/api/v1/apps"));
    const QJsonObject obj{
        {QStringLiteral("client_name"), appName},
        {QStringLiteral("redirect_uris"), m_redirectUri},
        {QStringLiteral("scopes"), QStringLiteral("read write follow %1").arg(m_additionalScopes)},
        {QStringLiteral("website"), website},
    };
    const QJsonDocument doc(obj);

    fetchInstanceMetadata();

#ifdef HAVE_KUNIFIEDPUSH
    connect(
        this,
        &AbstractAccount::authenticated,
        this,
        [this] {
            if (successfullyAuthenticated()) {
                // We asked for the push scope, so we can safely start subscribing to notifications
                config()->setEnablePushNotifications(true);
                config()->save();
            }
        },
        Qt::SingleShotConnection);
#endif

    post(regUrl, doc, false, this, [this](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        m_client_id = doc.object()["client_id"_L1].toString();
        m_client_secret = doc.object()["client_secret"_L1].toString();

        if (!m_client_id.isEmpty() && !m_client_secret.isEmpty()) {
            QMessageFilterContainer::self()->insert(m_client_secret, QStringLiteral("CLIENT_SECRET"));
            Q_EMIT registered();
        } else {
            // TODO: emit an error here
        }
    });
}

void AbstractAccount::registerAccount(const QString &username,
                                      const QString &email,
                                      const QString &password,
                                      bool agreement,
                                      const QString &locale,
                                      const QString &reason)
{
    // get an app-level access token, obviously we don't have a user token yet.
    const QUrl tokenUrl = apiUrl(QStringLiteral("/oauth/token"));
    QUrlQuery q;

    q.addQueryItem(QStringLiteral("client_id"), m_client_id);
    q.addQueryItem(QStringLiteral("client_secret"), m_client_secret);
    q.addQueryItem(QStringLiteral("grant_type"), QStringLiteral("client_credentials"));
    q.addQueryItem(QStringLiteral("scope"), QStringLiteral("write"));

    post(tokenUrl, q, false, this, [this, username, email, password, agreement, locale, reason](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);

        // We do not use setAccessToken here because it's a temporary one, and cannot be validated.
        m_token = doc.object()["access_token"_L1].toString();

        const QUrlQuery formdata{
            {QStringLiteral("username"), username},
            {QStringLiteral("email"), email},
            {QStringLiteral("password"), password},
            {QStringLiteral("agreement"), agreement ? QStringLiteral("1") : QStringLiteral("0")},
            {QStringLiteral("locale"), locale},
            {QStringLiteral("reason"), reason},
        };

        post(
            apiUrl(QStringLiteral("/api/v1/accounts")),
            formdata,
            true,
            this,
            [this, username](QNetworkReply *reply) {
                const auto data = reply->readAll();
                const auto doc = QJsonDocument::fromJson(data);

                if (doc.object().contains("access_token"_L1)) {
                    setUsername(username);
                    setAccessToken(doc["access_token"_L1].toString());
                }
            },
            [this](QNetworkReply *reply) {
                Q_EMIT registrationError(QString::fromUtf8(reply->readAll()));
            });
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

AdminAccountInfo *AbstractAccount::adminIdentityLookupWithVanillaPointer(const QString &accountId, const QJsonObject &doc)
{
    const auto id = new AdminAccountInfo();

    id->reparentAdminAccountInfo(this);
    id->fromSourceData(doc);
    m_adminIdentityCacheWithVanillaPointer[accountId] = id;

    return m_adminIdentityCacheWithVanillaPointer[accountId];
}

std::shared_ptr<ReportInfo> AbstractAccount::reportInfoLookup(const QString &reportId, const QJsonObject &doc)
{
    if (m_reportInfo && m_reportInfo->reportId() == reportId) {
        return m_reportInfo;
    }
    auto id = m_reportInfoCache[reportId];
    if (id && id->reportId() == reportId) {
        return id;
    }

    id = std::make_shared<ReportInfo>();
    id->reparentReportInfo(this);
    id->fromSourceData(doc);
    m_reportInfoCache[reportId] = id;

    return m_reportInfoCache[reportId];
}

bool AbstractAccount::identityCached(const QString &accountId) const
{
    if (m_identity && m_identity->id() == accountId) {
        return true;
    }
    auto id = m_identityCache[accountId];
    return id && id->id() == accountId;
}

QUrl AbstractAccount::getAuthorizeUrl() const
{
    QUrl url = apiUrl(QStringLiteral("/oauth/authorize"));
    QUrlQuery q;

    q.addQueryItem(QStringLiteral("client_id"), m_client_id);
    q.addQueryItem(QStringLiteral("redirect_uri"), m_redirectUri);
    q.addQueryItem(QStringLiteral("response_type"), QStringLiteral("code"));
    q.addQueryItem(QStringLiteral("scope"), QStringLiteral("read write follow ") + m_additionalScopes);

    url.setQuery(q);

    return url;
}

void AbstractAccount::setAccessToken(const QString &token)
{
    m_token = token;
    QMessageFilterContainer::self()->insert(m_token, QStringLiteral("ACCESS_TOKEN"));
    validateToken();
}

void AbstractAccount::setInstanceUri(const QString &instance_uri)
{
    // instance URI changed, get new credentials
    QUrl instance_url = QUrl::fromUserInput(instance_uri);
    instance_url.setScheme(QStringLiteral("https")); // getting token from http is not supported

    m_instance_uri = instance_url.toString();
}

QString AbstractAccount::instanceUri() const
{
    return m_instance_uri;
}

void AbstractAccount::setToken(const QString &authcode)
{
    const QUrl tokenUrl = apiUrl(QStringLiteral("/oauth/token"));
    QUrlQuery q;

    q.addQueryItem(QStringLiteral("client_id"), m_client_id);
    q.addQueryItem(QStringLiteral("client_secret"), m_client_secret);
    q.addQueryItem(QStringLiteral("redirect_uri"), m_redirectUri);
    q.addQueryItem(QStringLiteral("grant_type"), QStringLiteral("authorization_code"));
    q.addQueryItem(QStringLiteral("code"), authcode);

    post(tokenUrl, q, false, this, [this](QNetworkReply *reply) {
        auto data = reply->readAll();
        auto doc = QJsonDocument::fromJson(data);

        setAccessToken(doc.object()["access_token"_L1].toString());
    });
}

void AbstractAccount::mutatePost(const QString &id, const QString &verb, bool deliver_home)
{
    const QUrl mutation_url = apiUrl(QStringLiteral("/api/v1/statuses/%1/%2").arg(id, verb));
    const QJsonDocument doc;

    post(mutation_url, doc, true, this, [this, deliver_home](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (deliver_home) {
            QList<Post *> posts;
            auto obj = doc.object();

            auto p = new Post(this, obj, this);
            posts.push_back(p);

            Q_EMIT fetchedTimeline(QStringLiteral("home"), posts);
        }
    });
}

void AbstractAccount::favorite(Post *p)
{
    mutatePost(p->postId(), QStringLiteral("favourite"));
}

void AbstractAccount::unfavorite(Post *p)
{
    mutatePost(p->postId(), QStringLiteral("unfavourite"));
}

void AbstractAccount::repeat(Post *p)
{
    mutatePost(p->postId(), QStringLiteral("reblog"), true);
}

void AbstractAccount::unrepeat(Post *p)
{
    mutatePost(p->postId(), QStringLiteral("unreblog"));
}

void AbstractAccount::bookmark(Post *p)
{
    mutatePost(p->postId(), QStringLiteral("bookmark"), true);
}

void AbstractAccount::unbookmark(Post *p)
{
    mutatePost(p->postId(), QStringLiteral("unbookmark"));
}

void AbstractAccount::pin(Post *p)
{
    mutatePost(p->postId(), QStringLiteral("pin"), true);
}

void AbstractAccount::unpin(Post *p)
{
    mutatePost(p->postId(), QStringLiteral("unpin"));
}

void AbstractAccount::fetchInstanceMetadata()
{
    get(
        apiUrl(QStringLiteral("/api/v2/instance")),
        false,
        this,
        [this](QNetworkReply *reply) {
            const auto data = reply->readAll();
            const auto doc = QJsonDocument::fromJson(data);

            if (!doc.isObject()) {
                return;
            }

            const auto obj = doc.object();

            if (obj.contains("configuration"_L1)) {
                const auto configObj = obj["configuration"_L1].toObject();

                if (configObj.contains("statuses"_L1)) {
                    const auto statusConfigObj = configObj["statuses"_L1].toObject();
                    m_maxPostLength = statusConfigObj["max_characters"_L1].toInt();
                    m_charactersReservedPerUrl = statusConfigObj["characters_reserved_per_url"_L1].toInt();
                }
            }

            // Pleroma/Akkoma may report maximum post characters here, instead
            if (obj.contains("max_toot_chars"_L1)) {
                m_maxPostLength = obj["max_toot_chars"_L1].toInt();
            }

            // Pleroma/Akkoma can report higher poll limits
            if (obj.contains("poll_limits"_L1)) {
                m_maxPollOptions = obj["poll_limits"_L1].toObject()["max_options"_L1].toInt();
            }

            // Other instance of poll options
            if (obj.contains("polls"_L1)) {
                m_maxPollOptions = obj["polls"_L1].toObject()["max_options"_L1].toInt();
            }

            if (obj.contains("registrations"_L1)) {
                m_registrationsOpen = obj["registrations"_L1].toObject()["enabled"_L1].toBool();
                m_registrationMessage = obj["registrations"_L1].toObject()["message"_L1].toString();
            }

            m_supportsLocalVisibility = obj.contains("pleroma"_L1);

            m_instance_name = obj["title"_L1].toString();

            Q_EMIT fetchedInstanceMetadata();
        },
        [this](QNetworkReply *) {
            // Fall back to v1 instance information
            // TODO: a lot of this can be merged with v2 handling
            get(apiUrl(QStringLiteral("/api/v1/instance")), false, this, [this](QNetworkReply *reply) {
                const auto data = reply->readAll();
                const auto doc = QJsonDocument::fromJson(data);

                if (!doc.isObject()) {
                    return;
                }

                const auto obj = doc.object();

                if (obj.contains("configuration"_L1)) {
                    const auto configObj = obj["configuration"_L1].toObject();

                    if (configObj.contains("statuses"_L1)) {
                        const auto statusConfigObj = configObj["statuses"_L1].toObject();
                        m_maxPostLength = statusConfigObj["max_characters"_L1].toInt();
                        m_charactersReservedPerUrl = statusConfigObj["characters_reserved_per_url"_L1].toInt();
                    }
                }

                // Pleroma/Akkoma may report maximum post characters here, instead
                if (obj.contains("max_toot_chars"_L1)) {
                    m_maxPostLength = obj["max_toot_chars"_L1].toInt();
                }

                // Pleroma/Akkoma can report higher poll limits
                if (obj.contains("poll_limits"_L1)) {
                    m_maxPollOptions = obj["poll_limits"_L1].toObject()["max_options"_L1].toInt();
                }

                // Other instance of poll options
                if (obj.contains("polls"_L1)) {
                    m_maxPollOptions = obj["polls"_L1].toObject()["max_options"_L1].toInt();
                }

                m_registrationsOpen = obj["registrations"_L1].toBool();

                m_supportsLocalVisibility = obj.contains("pleroma"_L1);

                m_instance_name = obj["title"_L1].toString();

                Q_EMIT fetchedInstanceMetadata();
            });
        });

    fetchCustomEmojis();
}

void AbstractAccount::saveTimelinePosition(const QString &timeline, const QString &lastReadId)
{
    QUrl uri = apiUrl(QStringLiteral("/api/v1/markers"));

    QUrlQuery urlQuery(uri);
    urlQuery.addQueryItem(QStringLiteral("%1[last_read_id]").arg(timeline), lastReadId);
    uri.setQuery(urlQuery);

    post(uri, urlQuery, true, this, {});
}

QUrl AbstractAccount::streamingUrl(const QString &stream)
{
    QUrl url = apiUrl(QStringLiteral("/api/v1/streaming"));
    url.setQuery(QUrlQuery{
        {QStringLiteral("access_token"), m_token},
        {QStringLiteral("stream"), stream},
    });
    url.setScheme(QStringLiteral("wss"));

    return url;
}

void AbstractAccount::handleNotification(const QJsonDocument &doc)
{
    const auto obj = doc.object();
    std::shared_ptr<Notification> n = std::make_shared<Notification>(this, obj);

    if (n->type() == Notification::FollowRequest) {
        m_followRequestCount++;
        Q_EMIT followRequestCountChanged();
    } else {
        m_unreadNotificationsCount++;
        Q_EMIT unreadNotificationsCountChanged();
    }

    Q_EMIT notification(n);
}

void AbstractAccount::executeAction(Identity *identity, AccountAction accountAction, const QJsonObject &extraArguments)
{
    const QHash<AccountAction, QString> accountActionMap = {
        {AccountAction::Follow, QStringLiteral("/follow")},
        {AccountAction::Unfollow, QStringLiteral("/unfollow")},
        {AccountAction::RemoveFollower, QStringLiteral("/remove_from_followers")},
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
    const QString accountApiUrl = QStringLiteral("/api/v1/accounts/%1%2").arg(accountId, apiCall);
    const QJsonDocument doc(extraArguments);

    post(apiUrl(accountApiUrl), doc, true, this, [this, accountAction, identity](QNetworkReply *reply) {
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto jsonObj = doc.object();

        // Check if the request failed due to one account blocking the other
        if (!jsonObj.value("error"_L1).isUndefined()) {
            const QHash<AccountAction, QString> accountActionMap = {
                {AccountAction::Follow, i18n("Could not follow account")},
                {AccountAction::Unfollow, i18n("Could not unfollow account")},
                {AccountAction::RemoveFollower, i18n("Could not remove account as your follower")},
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
        if (relationship != nullptr) {
            relationship->updateFromJson(jsonObj);
        }

        Q_EMIT identity->relationshipChanged();
    });
}

int AbstractAccount::followRequestCount() const
{
    return m_followRequestCount;
}

void AbstractAccount::resetUnreadNotificationsCount()
{
    m_unreadNotificationsCount = 0;
    Q_EMIT unreadNotificationsCountChanged();
}

int AbstractAccount::unreadNotificationsCount() const
{
    return m_unreadNotificationsCount;
}

void AbstractAccount::followAccount(Identity *identity, bool reblogs, bool notify)
{
    executeAction(identity,
                  AccountAction::Follow,
                  {
                      {QStringLiteral("reblogs"), reblogs},
                      {QStringLiteral("notify"), notify},
                  });
}

void AbstractAccount::unfollowAccount(Identity *identity)
{
    executeAction(identity, AccountAction::Unfollow);
}

void AbstractAccount::removeFollower(Identity *identity)
{
    executeAction(identity, AccountAction::RemoveFollower);
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
    executeAction(identity, AccountAction::Mute, {{QStringLiteral("notifcations"), notifications}, {QStringLiteral("duration"), duration}});
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
        executeAction(identity, AccountAction::Note, {{QStringLiteral("comment"), note}});
    }
}

void AbstractAccount::mutateRemotePost(const QString &url, const QString &verb)
{
    requestRemoteObject(QUrl(url), this, [this, verb](QNetworkReply *reply) {
        const auto searchResult = QJsonDocument::fromJson(reply->readAll()).object();
        const auto statuses = searchResult[QStringLiteral("statuses")].toArray();
        const auto accounts = searchResult[QStringLiteral("accounts")].toArray();

        // TODO: emit error when the mutation has failed, no post is available on this account's server
        if (!statuses.isEmpty()) {
            const auto status = statuses[0].toObject();

            if (verb == QStringLiteral("reply")) {
                // TODO: we can't delete this immediately, will need some smarter cleanup in the PostEditorBackend
                Post *post = new Post(this, this);
                post->fromJson(status);
                Q_EMIT Navigation::instance().replyTo(post);
            } else {
                const QString localID = status["id"_L1].toString();
                mutatePost(localID, verb);
            }
        }
    });
}

void AbstractAccount::fetchOEmbed(const QString &id, Identity *identity)
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("url"), QStringLiteral("%1/@%2/%3").arg(m_instance_uri, identity->username(), id));

    QUrl oembedUrl = apiUrl(QStringLiteral("/api/oembed"));
    oembedUrl.setQuery(query);

    get(oembedUrl, false, this, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.object().contains("html"_L1)) {
            Q_EMIT fetchedOEmbed(doc.object()["html"_L1].toString());
        }
    });
}

bool AbstractAccount::registrationsOpen() const
{
    return m_registrationsOpen;
}

QString AbstractAccount::registrationMessage() const
{
    return m_registrationMessage;
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

    get(apiUrl(QStringLiteral("/api/v1/custom_emojis")), true, this, [this](QNetworkReply *reply) {
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

void AbstractAccount::addFavoriteList(const QString &id)
{
    auto ids = config()->favoriteListIds();
    ids.push_back(id);
    config()->setFavoriteListIds(ids);
    config()->save();
    Q_EMIT favoriteListsChanged();
}

void AbstractAccount::removeFavoriteList(const QString &id)
{
    auto ids = config()->favoriteListIds();
    ids.removeAll(id);
    config()->setFavoriteListIds(ids);
    config()->save();
    Q_EMIT favoriteListsChanged();
}

bool AbstractAccount::isFavoriteList(const QString &id)
{
    return config()->favoriteListIds().contains(id);
}

#include "moc_abstractaccount.cpp"
