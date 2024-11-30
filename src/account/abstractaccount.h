// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "account/identity.h"
#include "account/preferences.h"
#include "accountconfig.h"
#include "admin/adminaccountinfo.h"
#include "admin/reportinfo.h"
#include "utils/customemoji.h"

#include <QJsonObject>
#include <QtQml/qqmlregistration.h>

class Notification;
class QNetworkReply;
class QHttpMultiPart;
class Preferences;

/**
 * @brief Represents an account, which could possibly be real or a mock for testing.
 *
 * Also handles most of the API work, and account actions.
 */
class AbstractAccount : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Abstract class")

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)

    Q_PROPERTY(QString instanceUri READ instanceUri CONSTANT)
    Q_PROPERTY(int maxPostLength READ maxPostLength NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(int maxPollOptions READ maxPollOptions NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(bool supportsLocalVisibility READ supportsLocalVisibility NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QString instanceName READ instanceName NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QUrl authorizeUrl READ getAuthorizeUrl NOTIFY registered)
    Q_PROPERTY(Identity *identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(Preferences *preferences READ preferences CONSTANT)
    Q_PROPERTY(int followRequestCount READ followRequestCount NOTIFY followRequestCountChanged)
    Q_PROPERTY(AccountConfig *config READ config NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(bool registrationsOpen READ registrationsOpen NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QString registrationMessage READ registrationMessage NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(int unreadNotificationsCount READ unreadNotificationsCount NOTIFY unreadNotificationsCountChanged)

public:
    /**
     * @brief Register a new account on the server.
     * @param username The account's username.
     * @param email The account's email address.
     * @param password The account's password.
     * @param agreement Whether the user agrees the server's rules and terms.
     * @param locale The user's locale.
     * @param reason If the server requires approval, a reason given to register.
     */
    Q_INVOKABLE void
    registerAccount(const QString &username, const QString &email, const QString &password, bool agreement, const QString &locale, const QString &reason);

    /**
     * @return If the application is registered
     * @sa registerApplication
     */
    [[nodiscard]] bool isRegistered() const;

    /**
     * @return If this account's instance has registrations open.
     */
    [[nodiscard]] bool registrationsOpen() const;

    /**
     * @return The reason why registrations are disabled.
     */
    [[nodiscard]] QString registrationMessage() const;

    /**
     * @return The oauth2 authorization url.
     */
    Q_INVOKABLE [[nodiscard]] QUrl getAuthorizeUrl() const;

    /**
     * @brief Sets the access token.
     * @param token The access token.
     */
    void setAccessToken(const QString &token);

    /**
     * @brief Set the oauth2 token.
     * @param authcode The oauth2 authentication code.
     */
    Q_INVOKABLE void setToken(const QString &authcode);

    /**
     * @return If the account has a token set.
     * @see setToken()
     */
    [[nodiscard]] bool haveToken() const;

    /**
     * @return If the account has a username yet.
     */
    [[nodiscard]] bool hasName() const;

    /**
     * @brief Verifies the token with the instance and if successful, loads identity information for the account.
     */
    Q_INVOKABLE virtual void validateToken(bool newAccount = false) = 0;

    /**
     * @return The local account preferences.
     */
    AccountConfig *config();

    /**
     * @return The server-side preferences.
     */
    [[nodiscard]] Preferences *preferences() const;

    /**
     * @return The username of the account.
     * @see setUsername()
     */
    [[nodiscard]] QString username() const;

    /**
     * @brief Fetches instance-specific metadata like max post length, allowed content types, etc.
     */
    void fetchInstanceMetadata();

    /**
     * @brief Fetches instance-specific custom emojis.
     */
    void fetchCustomEmojis();

    /**
     * @return The custom emojis that's accessible for this account.
     */
    [[nodiscard]] QList<CustomEmoji> customEmojis() const;

    /**
     * @return The instance URI.
     * @see setInstanceUri()
     */
    [[nodiscard]] QString instanceUri() const;

    /**
     * @return The max allowable length of posts in characters.
     */
    [[nodiscard]] size_t maxPostLength() const;

    /**
     * @return The maximum number of poll options.
     */
    [[nodiscard]] size_t maxPollOptions() const;

    /**
     * @return Certain servers (like Pleroma/Akkoma) support an additional visibility type, called Local.
     * @sa Post::Visibility
     */
    [[nodiscard]] bool supportsLocalVisibility() const;

    /**
     * @return Returns the amount of characters that URLs take. Any URL that appears in a post will only be counted by this limit.
     */
    [[nodiscard]] size_t charactersReservedPerUrl() const;

    /**
     * @return The title set by the instance.
     */
    [[nodiscard]] QString instanceName() const;

    /**
     * @return The identity of the account.
     */
    Identity *identity();

    /**
     * @brief Looks up an identity specific to this account (like relationships) using an accountId and optionally a JSON document containing identity
     * information.
     * @param accountId The account ID.
     * @param doc Optionally provide an existing account JSON, if you were already given some in another request.
     * @return The requested identity.
     */
    std::shared_ptr<Identity> identityLookup(const QString &accountId, const QJsonObject &doc);

    /**
     * @brief Checks if the accountId exists in the account's identity cache.
     * @param accountId The account ID to look up.
     * @return If the identity was cached.
     */
    [[nodiscard]] bool identityCached(const QString &accountId) const;

    /**
     * Get identity of the admin::account.
     * @param accountId The account ID to look up.
     * @param doc Optionally provide an existing account JSON, if you were already given some in another request.
     * @return The requested admin account info.
     */
    std::shared_ptr<AdminAccountInfo> adminIdentityLookup(const QString &accountId, const QJsonObject &doc);

    /**
     * @brief Vanilla pointer identity.
     */
    AdminAccountInfo *adminIdentityLookupWithVanillaPointer(const QString &accountId, const QJsonObject &doc);

    /**
     * @brief Populating with Admin::Report.
     */
    std::shared_ptr<ReportInfo> reportInfoLookup(const QString &reportId, const QJsonObject &doc);

    /**
     * @brief Invalidates this account.
     */
    void invalidate();

    /**
     * @brief Saves the timeline position.
     * @param timeline Which timeline to save, right now can only be "home" or "notifications"
     * @param lastReadId The last read post id.
     */
    Q_INVOKABLE void saveTimelinePosition(const QString &timeline, const QString &lastReadId);

    /**
     * @brief Favorite a post.
     * @param p The post object to mutate.
     * @see unfavorite()
     */
    void favorite(Post *p);

    /**
     * @brief Unfavorite a post.
     * @param p The post object to mutate.
     * @see favorite()
     */
    void unfavorite(Post *p);

    /**
     * @brief Boost (also known as reblog, or repeat) a post
     * @param p The post object to mutate.
     * @see unrepeat()
     */
    void repeat(Post *p);

    /**
     * @brief Unboost a post.
     * @param p The post object to mutate.
     * @see repeat()
     */
    void unrepeat(Post *p);

    /**
     * @brief Bookmark a post.
     * @param p The post object to mutate.
     * @see unbookmark()
     */
    void bookmark(Post *p);

    /**
     * @brief Unbookmark a post.
     * @param p The post object to mutate.
     * @see bookmark()
     */
    void unbookmark(Post *p);

    /**
     * @brief Pin a post.
     * @param p The post object to mutate.
     * @see unpin()
     */
    void pin(Post *p);

    /**
     * @brief Unpin a post.
     * @param p The post object to mutate.
     * @see pin()
     */
    void unpin(Post *p);

    /**
     * @brief Returns a streaming url for @p stream.
     * @param stream The requested stream (e.g. user).
     */
    QUrl streamingUrl(const QString &stream);

    /**
     * @brief Invalidates a post.
     * @param p The post object to mutate.
     */
    void invalidatePost(Post *p);

    /**
     * @param path The base API path.
     * @return A well-formed URL of an API path.
     */
    [[nodiscard]] QUrl apiUrl(const QString &path) const;

    /**
     * @brief Make an HTTP GET request to the server.
     * @param url The url of the request.
     * @param authenticated Whether the request should be authenticated.
     * @param parent The parent object that calls get() or the callback belongs to.
     * @param callback The callback that should be executed if the request is successful.
     * @param errorCallback The callback that should be executed if the request is not successful.
     */
    virtual void get(const QUrl &url,
                     bool authenticated,
                     QObject *parent,
                     std::function<void(QNetworkReply *)> callback,
                     std::function<void(QNetworkReply *)> errorCallback = nullptr) = 0;

    /**
     * @brief Make an HTTP POST request to the server.
     * @param url The url of the request.
     * @param doc The request body as JSON.
     * @param authenticated Whether the request should be authenticated.
     * @param parent The parent object that calls get() or the callback belongs to.
     * @param callback The callback that should be executed if the request is successful.
     * @param errorCallback The callback that should be executed if the request is not successful.
     * @param headers
     */
    virtual void post(const QUrl &url,
                      const QJsonDocument &doc,
                      bool authenticated,
                      QObject *parent,
                      std::function<void(QNetworkReply *)> callback,
                      std::function<void(QNetworkReply *)> errorCallback = nullptr,
                      QHash<QByteArray, QByteArray> headers = {}) = 0;

    /**
     * @brief Make an HTTP POST request to the server.
     * @param url The url of the request.
     * @param formdata The request body as form-data.
     * @param authenticated Whether the request should be authenticated.
     * @param parent The parent object that calls get() or the callback belongs to.
     * @param callback The callback that should be executed if the request is successful.
     * @param errorCallback The callback that should be executed if the request is not successful.
     */
    virtual void post(const QUrl &url,
                      const QUrlQuery &formdata,
                      bool authenticated,
                      QObject *parent,
                      std::function<void(QNetworkReply *)> callback,
                      std::function<void(QNetworkReply *)> errorCallback = nullptr) = 0;

    /**
     * @brief Make an HTTP POST request to the server.
     * @param url The url of the request.
     * @param message The request body as multi-part data.
     * @param authenticated Whether the request should be authenticated.
     * @param parent The parent object that calls get() or the callback belongs to.
     * @param callback The callback that should be executed if the request is successful.
     * @return
     */
    virtual QNetworkReply *
    post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /**
     * @brief Make an HTTP PUT request to the server.
     * @param url The url of the request.
     * @param doc The request body as JSON.
     * @param authenticated Whether the request should be authenticated.
     * @param parent The parent object that calls get() or the callback belongs to.
     * @param callback The callback that should be executed if the request is successful.
     */
    virtual void put(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /**
     * @brief Make an HTTP PUT request to the server.
     * @param url The url of the request.
     * @param doc The request body as form-data.
     * @param authenticated Whether the request should be authenticated.
     * @param parent The parent object that calls get() or the callback belongs to.
     * @param callback The callback that should be executed if the request is successful.
     */
    virtual void put(const QUrl &url, const QUrlQuery &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /**
     * @brief Make an HTTP PATCH request to the server.
     * @param url The url of the request.
     * @param multiPart The request body as multi-part data.
     * @param authenticated Whether the request should be authenticated.
     * @param parent The parent object that calls get() or the callback belongs to.
     */
    virtual void patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)>) = 0;

    /**
     * @brief Make an HTTP DELETE request to the server.
     * @param url The url of the request.
     * @param authenticated Whether the request should be authenticated.
     * @param parent The parent object that calls get() or the callback belongs to.
     * @param callback The callback that should be executed if the request is successful.
     */
    virtual void deleteResource(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /**
     * @brief Upload a file to the server.
     * @param filename The name of the file to upload.
     * @param callback The callback that should be executed if the request is successful.
     * @return
     */
    virtual QNetworkReply *upload(const QUrl &filename, std::function<void(QNetworkReply *)> callback) = 0;

    /**
     * @brief Find the remote URL on this account's server. For example, giving it a post @p url will give the equivalent post on this server if available.
     * @param url The URL of the object to retrieve.
     * @param parent The parent object for this callback.
     * @param callback The callback when the remote object is found. Usually a JSON document.
     */
    virtual void requestRemoteObject(const QUrl &url, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /**
     * @brief Write account to settings to disk.
     */
    virtual void writeToSettings() = 0;

    /**
     * @brief Read account from settings to disk.
     */
    virtual void buildFromSettings() = 0;

    /**
     * @return If the account has any follow requests.
     */
    [[nodiscard]] int followRequestCount() const;

    /**
     * @brief Check against the server for any new follow requests.
     */
    virtual void checkForFollowRequests() = 0;

    /**
     * @brief Update the number of unread notifications from the server.
     */
    virtual void checkForUnreadNotifications() = 0;

    /**
     * @brief Resets the unread notifications count *only* on the client-side.
     *
     * Use checkForUnreadNotifications() if you want to check if it's reset on the server.
     */
    void resetUnreadNotificationsCount();

    [[nodiscard]] int unreadNotificationsCount() const;

    /**
     * @brief Update the push notification rules.
     */
    virtual void updatePushNotifications() = 0;

    /**
     * @brief Follow the given account. Can also be used to update whether to show reblogs or enable notifications.
     * @param identity The account to follow.
     * @param reblogs Receive this account's reblogs in home timeline? Defaults to true.
     * @param notify Receive notifications when this account posts a status? Defaults to false.
     */
    Q_INVOKABLE void followAccount(Identity *identity, bool reblogs = true, bool notify = false);

    /**
     * @brief Unfollow the given account.
     * @param identity The account to unfollow.
     */
    Q_INVOKABLE void unfollowAccount(Identity *identity);

    /**
     * @brief Remove the given account as your follower.
     * @param identity The account to remove as follower.
     */
    Q_INVOKABLE void removeFollower(Identity *identity);

    /**
     * @brief Block the given account.
     * @param identity The account to block.
     */
    Q_INVOKABLE void blockAccount(Identity *identity);

    /**
     * @brief Unblock the given account.
     * @param identity The account to unblock.
     */
    Q_INVOKABLE void unblockAccount(Identity *identity);

    /**
     * @brief Mute the given account.
     * @param identity The account to mute.
     * @param notifications Whether notifications should also be muted, by default true.
     * @param duration How long the mute should last, in seconds. Defaults to 0 (indefinite).
     */
    Q_INVOKABLE void muteAccount(Identity *identity, bool notifications = true, int duration = 0);

    /**
     * @brief Unmute the given account.
     * @param identity The account to unmute.
     */
    Q_INVOKABLE void unmuteAccount(Identity *identity);

    /**
     * @brief Add the given account to the user's featured profiles.
     * @param identity The account to feature.
     */
    Q_INVOKABLE void featureAccount(Identity *identity);

    /**
     * @brief Remove the given account from the user's featured profiles.
     * @param identity The account to unfeature.
     */
    Q_INVOKABLE void unfeatureAccount(Identity *identity);

    /**
     * @brief Sets a private note on a user.
     * @param identity The account to annotate.
     * @param note The note to add to the account. Leave empty to remove the existing note.
     */
    Q_INVOKABLE void addNote(Identity *identity, const QString &note);

    Q_INVOKABLE void mutateRemotePost(const QString &url, const QString &verb);

    /**
     * @brief Fetches OEmbed data for a post.
     */
    Q_INVOKABLE void fetchOEmbed(const QString &id, Identity *identity);

    /**
     * @return The preferred settings group name for this Account which includes the username and the instance uri.
     */
    [[nodiscard]] QString settingsGroupName() const;

    /**
     * @return The preferred key name for the client secret.
     */
    [[nodiscard]] QString clientSecretKey() const;

    /**
     * @return The preferred key name for the access token.
     */
    [[nodiscard]] QString accessTokenKey() const;

    /**
     * @brief Type of account action.
     */
    enum AccountAction {
        Follow, /**< Follow the account. */
        Unfollow, /**< Unfollow the account. */
        RemoveFollower, /**< Remove the account as your follower. */
        Block, /**< Block the account. */
        Unblock, /**< Unlock the account. */
        Mute, /**< Mute the account. */
        Unmute, /**< Unmute the account. */
        Feature, /**< Feature the account. */
        Unfeature, /**< Unfeature the account. */
        Note, /**< Update the note for the account. */
    };

    /**
     * @brief Type of streaming event.
     */
    enum StreamingEventType {
        UpdateEvent, /**< A new Status has appeared. */
        DeleteEvent, /**< A status has been deleted. */
        NotificationEvent, /**< A new notification has appeared. */
        FiltersChangedEvent, /**< Keyword filters have been changed. */
        ConversationEvent, /**< A direct conversation has been updated. */
        AnnouncementEvent, /**< An announcement has been published. */
        AnnouncementRedactedEvent, /**< An announcement has received an emoji reaction. */
        AnnouncementDeletedEvent, /**< An announcement has been deleted. */
        StatusUpdatedEvent, /**< A Status has been edited. */
        EncryptedMessageChangedEvent, /**< An encrypted message has been received. */
    };

Q_SIGNALS:
    /**
     * @brief Emitted when the account is authenticated.
     * @param successful Whether the authentication was successful.
     * @param errorMessage If not successful, a localized error message.
     * @see validateToken()
     */
    void authenticated(bool successful, const QString &errorMessage);

    /**
     * @brief Emitted when the application is successfully registered to the server
     * @see registerApplication()
     */
    void registered();

    /**
     * @brief Emitted when the account's own identity has been updated.
     */
    void identityChanged();

    /**
     * @brief Emitted when the requested timeline has been fetched.
     * @param timelineName name of the timeline that was fetched.
     * @param posts The list of posts fetched.
     */
    void fetchedTimeline(const QString &timelineName, QList<Post *> posts);

    /**
     * @brief Emitted when th=e account has been invalidated
     * @see invalidate()
     */
    void invalidated();

    /**
     * @brief Emitted when the account's username has been changed.
     * @see setUsername()
     */
    void usernameChanged();

    /**
     * @brief Emitted when the instance metadata has been fetched.
     * @see fetchInstanceMetadata()
     */
    void fetchedInstanceMetadata();

    /**
     * @brief Emitted when the custom emojis has been fetched.
     * @see fetchCustomEmojis()
     */
    void fetchedCustomEmojis();

    /**
     * @brief Emitted when a post has been invalidated.
     * @param p The post that was invalidated.
     * @see invalidatePost()
     */
    void invalidatedPost(Post *p);

    /**
     * @brief Emitted when a notification has been received.
     * @param n A shared handle to the new notification.
     */
    void notification(std::shared_ptr<Notification> n);

    /**
     * @brief Emitted when an error occurred when performing an API request.
     * @param errorMessage A localized error message.
     */
    void errorOccured(const QString &errorMessage);

    /**
     * @brief Emitted when a streaming event has been received
     * @param eventType The type of streaming event.
     * @param payload The payload for the streaming event.
     */
    void streamingEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload);

    /**
     * @brief Emitted when the number of follow requests was changed.
     */
    void followRequestCountChanged();

    /**
     * @brief Emitted when the number of unread notifications was changed.
     */
    void unreadNotificationsCountChanged();

    /**
     * @brief Emitted when a registration error has occurred.
     * @param json The JSON body for further processing.
     */
    void registrationError(const QString &json);

    /**
     * @brief Emitted when the oembed data is successfully returned.
     * @see fetchOEmbed()
     */
    void fetchedOEmbed(const QString &html);

protected:
    explicit AbstractAccount(const QString &instanceUri, QObject *parent = nullptr);

    /**
     * @brief Register the application on the server.
     * @param appName The name of the application displayed to other clients.
     * @param website The application's website.
     * @param additionalScopes Any additional scopes to request.
     * @param useAuthCode If the auth code method should be used instead of an ouath callback.
     */
    void registerApplication(const QString &appName, const QString &website, const QString &additionalScopes = {}, bool useAuthCode = false);

    /**
     * @brief Sets the username for the account.
     * @param name The new username.
     */
    void setUsername(const QString &name);

    /**
     * @brief Sets the instance URI for the account.
     * @param instance_uri The new instance URI.
     */
    void setInstanceUri(const QString &instance_uri);

    QString m_name;
    QString m_instance_uri;
    QString m_token;
    QString m_client_id;
    QString m_client_secret;
    size_t m_maxPostLength;
    size_t m_maxPollOptions;
    bool m_supportsLocalVisibility;
    size_t m_charactersReservedPerUrl;
    QString m_instance_name;
    QJsonArray m_instance_rules;
    std::shared_ptr<Identity> m_identity;
    std::shared_ptr<AdminAccountInfo> m_adminIdentity;
    AdminAccountInfo *m_adminIdentityWithVanillaPointer{};
    AdminAccountInfo *m_federationIdentity{};
    std::shared_ptr<ReportInfo> m_reportInfo;
    Preferences *m_preferences = nullptr;
    QList<CustomEmoji> m_customEmojis;
    QString m_additionalScopes;
    AccountConfig *m_config = nullptr;
    bool m_registrationsOpen = false;
    QString m_registrationMessage;
    int m_followRequestCount = 0;
    int m_unreadNotificationsCount = 0;
    QString m_redirectUri;

    // updates and notifications
    void handleNotification(const QJsonDocument &doc);

    void mutatePost(const QString &id, const QString &verb, bool deliver_home = false);
    QMap<QString, std::shared_ptr<Identity>> m_identityCache;
    QMap<QString, std::shared_ptr<AdminAccountInfo>> m_adminIdentityCache;
    QMap<QString, AdminAccountInfo *> m_adminIdentityCacheWithVanillaPointer;
    QMap<QString, std::shared_ptr<ReportInfo>> m_reportInfoCache;

    void executeAction(Identity *i, AccountAction accountAction, const QJsonObject &extraArguments = {});

    friend class MockAccount;
    friend class AccountTest;
    friend class ProfileEditorTest;
    friend class TimelineTest;
};
