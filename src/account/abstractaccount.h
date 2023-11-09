// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "accountconfig.h"
#include "adminaccountinfo.h"
#include "customemoji.h"
#include "identity.h"
#include "preferences.h"
#include "reportinfo.h"

#include <QJsonDocument>
#include <QJsonObject>

class Attachment;
class Notification;
class QNetworkReply;
class QHttpMultiPart;
class QFile;
class Preferences;

/// Represents an account, which could possibly be real or a mock for testing.
/// Also handles most of the API work, and account actions.
class AbstractAccount : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)

    Q_PROPERTY(QString instanceUri READ instanceUri CONSTANT)
    Q_PROPERTY(int maxPostLength READ maxPostLength NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(int maxPollOptions READ maxPollOptions NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(bool supportsLocalVisibility READ supportsLocalVisibility NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QString instanceName READ instanceName NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QUrl authorizeUrl READ getAuthorizeUrl NOTIFY registered)
    Q_PROPERTY(Identity *identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(Preferences *preferences READ preferences CONSTANT)
    Q_PROPERTY(bool hasFollowRequests READ hasFollowRequests NOTIFY hasFollowRequestsChanged)
    Q_PROPERTY(AccountConfig *config READ config NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(bool registrationsOpen READ registrationsOpen NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QString registrationMessage READ registrationMessage NOTIFY fetchedInstanceMetadata)

public:
    AbstractAccount(QObject *parent, const QString &instanceUri);
    AbstractAccount(QObject *parent);

    /// Register the application on the server
    /// \param appName The name of the application displayed to other clients
    /// \param website The application's website
    /// \param additionalScopes Any additional scopes to request
    void registerApplication(const QString &appName, const QString &website, const QString &additionalScopes = {});

    /// Register a new account on the server
    /// \param username The account's username
    /// \param email The account's email address
    /// \param password The account's password
    /// \param agreement Whether the user agrees the server's rules and terms
    /// \param locale The user's locale
    /// \param reason If the server requires approval, a reason given to register
    Q_INVOKABLE void
    registerAccount(const QString &username, const QString &email, const QString &password, bool agreement, const QString &locale, const QString &reason);

    /// Check if the application is registered
    /// \see registerApplication
    bool isRegistered() const;

    /// Check if this account's instance has registrations open.
    bool registrationsOpen() const;

    /// Check the reason why registrations are disabled,
    QString registrationMessage() const;

    /// Get the oauth2 authorization url
    Q_INVOKABLE QUrl getAuthorizeUrl() const;

    /// Sets the access token
    /// \param token The access token
    void setAccessToken(const QString &token);

    /// Get the oauth2 token url
    QUrl getTokenUrl() const;

    /// Set the oauth2 token
    /// \param authcode The oauth2 authentication code
    Q_INVOKABLE void setToken(const QString &authcode);

    /// Check if the account has a token set
    /// \see setToken
    bool haveToken() const;

    /// Check if the account has a username yet
    bool hasName() const;

    /// Check if the account has an instance uri set
    bool hasInstanceUrl() const;

    /// Verifies the token with the instance and if successful, loads identity information for the account
    Q_INVOKABLE virtual void validateToken() = 0;

    /// Return local account preferences
    AccountConfig *config();

    /// Returns the server-side preferences
    Preferences *preferences() const;

    /// Return the username of the account
    /// \see setUsername
    QString username() const;

    /// Sets the username for the account
    /// \param name The new username
    void setUsername(const QString &name);

    /// Fetches instance-specific metadata like max post length, allowed content types, etc
    void fetchInstanceMetadata();

    /// Fetches instance-specific custom emojis
    void fetchCustomEmojis();

    /// Returns the custom emojis that's accessible for this account
    QList<CustomEmoji> customEmojis() const;

    /// Returns the instance URI
    /// \see setInstanceUri
    QString instanceUri() const;

    /// Sets the instance URI for the account
    /// \param instance_uri The new instance URI
    void setInstanceUri(const QString &instance_uri);

    /// Returns the max allowable length of posts in characters
    size_t maxPostLength() const;

    /// Returns the maximum number of poll options
    size_t maxPollOptions() const;

    /// Certain servers (like Pleroma/Akkoma) support an additional visibility type, called Local
    bool supportsLocalVisibility() const;

    /// Returns the amount of characters that URLs take
    /// Any URL that appears in a post will only be counted by this limit
    size_t charactersReservedPerUrl() const;

    /// Returns the title set by the instance
    QString instanceName() const;

    /// Returns the identity of the account
    Identity *identity();

    /// Looks up an identity specific to this account (like relationships) using an accountId
    /// and optionally a JSON document containing identity information.
    /// \param accountId The account ID
    /// \param doc doc Optionally provide an existing account JSON, if you were already given some in another request
    std::shared_ptr<Identity> identityLookup(const QString &accountId, const QJsonObject &doc);

    /// Checks if the accountId exists in the account's identity cache
    /// \param accountId The account ID to look up
    bool identityCached(const QString &accountId) const;

    /// Get identity of the admin::account
    /// \param accountId The account ID to look up
    /// \param doc doc Optionally provide an existing account JSON, if you were already given some in another request
    std::shared_ptr<AdminAccountInfo> adminIdentityLookup(const QString &accountId, const QJsonObject &doc);

    /// Vanilla pointer identity
    AdminAccountInfo *adminIdentityLookupWithVanillaPointer(const QString &accountId, const QJsonObject &doc);

    /// Populating with Admin::Report
    std::shared_ptr<ReportInfo> reportInfoLookup(const QString &reportId, const QJsonObject &doc);

    /// Invalidates the account
    void invalidate();

    /// Favorite a post
    /// \param p The post object to mutate
    /// \see unfavorite
    void favorite(Post *p);

    /// Unfavorite a post
    /// \param p The post object to mutate
    /// \see favorite
    void unfavorite(Post *p);

    /// Boost (also known as reblog, or repeat) a post
    /// \param p The post object to mutate
    /// \see unrepeat
    void repeat(Post *p);

    /// Unboost a post
    /// \param p The post object to mutate
    /// \see repeat
    void unrepeat(Post *p);

    /// Bookmark a post
    /// \param p The post object to mutate
    /// \see unbookmark
    void bookmark(Post *p);

    /// Unbookmark a post
    /// \param p The post object to mutate
    /// \see bookmark
    void unbookmark(Post *p);

    /// Pin a post
    /// \param p The post object to mutate
    /// \see unpin
    void pin(Post *p);

    /// Unpin a post
    /// \param p The post object to mutate
    /// \see pin
    void unpin(Post *p);

    /// Returns a streaming url for \p stream
    /// \param stream The requested stream (e.g. user)
    QUrl streamingUrl(const QString &stream);

    /// Invalidates a post
    /// \param p The post object to mutate
    void invalidatePost(Post *p);

    /// Types of formatting that we may use is determined primarily by the server metadata, this is a simple enough
    /// way to determine what formats are accepted.
    enum AllowedContentType { PlainText = 1 << 0, Markdown = 1 << 1, Html = 1 << 2, BBCode = 1 << 3 };

    /// Return the allowed content types of the account's instance
    AllowedContentType allowedContentTypes() const
    {
        return m_allowedContentTypes;
    }

    /// Return a well-formed URL of an API path
    /// \param path The base API path
    QUrl apiUrl(const QString &path) const;

    /// Make an HTTP GET request to the server
    /// \param url The url of the request
    /// \param authenticated Whether the request should be authentificated
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    /// \param errorCallback The callback that should be executed if the request is not successful
    virtual void get(const QUrl &url,
                     bool authenticated,
                     QObject *parent,
                     std::function<void(QNetworkReply *)> callback,
                     std::function<void(QNetworkReply *)> errorCallback = nullptr) = 0;

    /// Make an HTTP POST request to the server
    /// \param url The url of the request
    /// \param doc The request body as JSON
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    /// \param errorCallback The callback that should be executed if the request is not successful
    virtual void post(const QUrl &url,
                      const QJsonDocument &doc,
                      bool authenticated,
                      QObject *parent,
                      std::function<void(QNetworkReply *)> callback,
                      std::function<void(QNetworkReply *)> errorCallback = nullptr,
                      QHash<QByteArray, QByteArray> headers = {}) = 0;

    /// Make an HTTP POST request to the server
    /// \param url The url of the request
    /// \param doc The request body as form-data
    /// \param authenticated Whether the request should be authenticated
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    /// \param errorCallback The callback that should be executed if the request is not successful
    virtual void post(const QUrl &url,
                      const QUrlQuery &formdata,
                      bool authenticated,
                      QObject *parent,
                      std::function<void(QNetworkReply *)> callback,
                      std::function<void(QNetworkReply *)> errorCallback = nullptr) = 0;

    /// Make an HTTP POST request to the server
    /// \param url The url of the request
    /// \param message The request body as multi-part data
    /// \param authenticated Whether the request should be authenticated
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    virtual QNetworkReply *post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /// Make an HTTP PUT request to the server
    /// \param url The url of the request
    /// \param doc The request body as JSON
    /// \param authenticated Whether the request should be authenticated
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    virtual void put(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /// Make an HTTP PUT request to the server
    /// \param url The url of the request
    /// \param doc The request body as form-data
    /// \param authenticated Whether the request should be authenticated
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    virtual void put(const QUrl &url, const QUrlQuery &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /// Make an HTTP PATCH request to the server
    /// \param url The url of the request
    /// \param message The request body as multi-part data
    /// \param authenticated Whether the request should be authenticated
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    virtual void patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)>) = 0;

    /// Make an HTTP DELETE request to the server
    /// \param url The url of the request
    /// \param authenticated Whether the request should be authenticated
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    virtual void deleteResource(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /// Upload a file
    /// \param url The name of the file to upload
    /// \param callback The callback that should be executed if the request is successful
    virtual QNetworkReply *upload(const QUrl &filename, std::function<void(QNetworkReply *)> callback) = 0;

    /// Write account to settings to disk
    virtual void writeToSettings() = 0;

    /// Read account from settings to disk
    virtual void buildFromSettings() = 0;

    /// Check if the account has any follow requests
    virtual bool hasFollowRequests() const = 0;

    /// Check against the server for any new follow requests
    virtual void checkForFollowRequests() = 0;

    /// Update the push notification rules
    virtual void updatePushNotifications() = 0;

    /// Follow the given account. Can also be used to update whether to show reblogs or enable notifications.
    /// @param identity The account to follow
    /// @param reblogs Receive this account's reblogs in home timeline? Defaults to true.
    /// @param notify Receive notifications when this account posts a status? Defaults to false.
    Q_INVOKABLE void followAccount(Identity *identity, bool reblogs = true, bool notify = false);

    /// Unfollow the given account.
    /// @param identity The account to unfollow
    Q_INVOKABLE void unfollowAccount(Identity *identity);

    /// Block the given account.
    /// @param identity The account to block
    Q_INVOKABLE void blockAccount(Identity *identity);

    /// Unblock the given account.
    /// @param identity The account to unblock
    Q_INVOKABLE void unblockAccount(Identity *identity);

    /// Mute the given account.
    /// @param identity The account to mute
    /// @param notifications Whether notifications should also be muted, by default true
    /// @param duration How long the mute should last, in seconds. Defaults to 0 (indefinite).
    Q_INVOKABLE void muteAccount(Identity *identity, bool notifications = true, int duration = 0);

    /// Unmute the given account.
    /// @param identity The account to unmute
    Q_INVOKABLE void unmuteAccount(Identity *identity);

    /// Add the given account to the user's featured profiles.
    /// @param identity The account to feature
    Q_INVOKABLE void featureAccount(Identity *identity);

    /// Remove the given account from the user's featured profiles.
    /// @param identity The account to unfeature
    Q_INVOKABLE void unfeatureAccount(Identity *identity);

    /// Sets a private note on a user.
    /// @param identity The account to annotate
    /// @param note The note to add to the account. Leave empty to remove the existing note.
    Q_INVOKABLE void addNote(Identity *identity, const QString &note);

    Q_INVOKABLE void mutateRemotePost(const QString &url, const QString &verb);

    /// Returns the preferred settings group name for this Account which includes the username and the instance uri.
    QString settingsGroupName() const;

    /// Returns the preferred key name for the client secret
    QString clientSecretKey() const;

    /// Returns the preferred key name for the access token
    QString accessTokenKey() const;

    /// Type of account action
    enum AccountAction {
        Follow, ///< Follow the account
        Unfollow, ///< Unfollow the account
        Block, ///< Block the account
        Unblock, ///< Unlock the account
        Mute, ///< Mute the account
        Unmute, ///< Unmute the account
        Feature, ///< Feature the account
        Unfeature, ///< Unfeature the account
        Note, ///< Update the note for the account
    };

    /// Type of streaming event
    enum StreamingEventType {
        UpdateEvent, ///< A new Status has appeared.
        DeleteEvent, ///< A status has been deleted.
        NotificationEvent, ///< A new notification has appeared.
        FiltersChangedEvent, ///< Keyword filters have been changed.
        ConversationEvent, ///< A direct conversation has been updated.
        AnnouncementEvent, ///< An announcement has been published.
        AnnouncementRedactedEvent, ///< An announcement has received an emoji reaction.
        AnnouncementDeletedEvent, ///< An announcement has been deleted.
        StatusUpdatedEvent, ///< A Status has been edited.
        EncryptedMessageChangedEvent, ///< An encrypted message has been received.
    };

Q_SIGNALS:
    /// Emitted when the account is authenticated
    /// \param Whether the authentication was successful
    /// \param errorMessage If not successful, a localized error message
    /// \see validateToken
    void authenticated(bool successful, const QString &errorMessage);

    /// Emitted when the application is successfully registered to the server
    /// \see registerApplication
    void registered();

    /// Emitted when the account's own identity has been updated
    void identityChanged();

    /// Emitted when the requested timeline has been fetched
    /// \param The name of the timeline that was fetched
    /// \param posts The list of posts fetched
    void fetchedTimeline(const QString &timelineName, QList<Post *> posts);

    /// Emitted when th=e account has been invalidated
    /// \see invalidate
    void invalidated();

    /// Emitted when the account's username has been changed
    /// \see setUsername
    void usernameChanged();

    /// Emitted when the instance metadata has been fetched
    /// \see fetchInstanceMetadata
    void fetchedInstanceMetadata();

    /// Emitted when the custom emojis has been fetched
    /// \see fetchCustomEmojis
    void fetchedCustomEmojis();

    /// Emitted when a post has been invalidated
    /// \param p The post that was invalidated
    /// \see invalidatePost
    void invalidatedPost(Post *p);

    /// Emitted when a notification has been received
    /// \param n A shared handle to the new notification
    void notification(std::shared_ptr<Notification> n);

    /// Emitted when an error occurred when performing an API request
    /// \param errorMessage A localized error message
    void errorOccured(const QString &errorMessage);

    /// Emitted when a streaming event has been received
    /// \param eventType The type of streaming event
    /// \param payload The payload for the streaming event
    void streamingEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload);

    /// Emitted when the account has follow requests
    void hasFollowRequestsChanged();

    /// Emitted when a registration error has occurred.
    /// \param json The JSON body for further processing
    void registrationError(const QString &json);

protected:
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
    AdminAccountInfo *m_adminIdentityWithVanillaPointer;
    AdminAccountInfo *m_federationIdentity;
    std::shared_ptr<ReportInfo> m_reportInfo;
    AllowedContentType m_allowedContentTypes;
    Preferences *m_preferences = nullptr;
    QList<CustomEmoji> m_customEmojis;
    QString m_additionalScopes;
    AccountConfig *m_config = nullptr;
    bool m_registrationsOpen = false;
    QString m_registrationMessage;
    bool m_hasFollowRequests = false;

    // OAuth authorization
    QUrlQuery buildOAuthQuery() const;

    // updates and notifications
    void handleUpdate(const QJsonDocument &doc, const QString &target);
    void handleNotification(const QJsonDocument &doc);

    void mutatePost(const QString &id, const QString &verb, bool deliver_home = false);
    QMap<QString, std::shared_ptr<Identity>> m_identityCache;
    QMap<QString, std::shared_ptr<AdminAccountInfo>> m_adminIdentityCache;
    QMap<QString, AdminAccountInfo *> m_adminIdentityCacheWithVanillaPointer;
    QMap<QString, std::shared_ptr<ReportInfo>> m_reportInfoCache;

    void executeAction(Identity *i, AccountAction accountAction, const QJsonObject &extraArguments = {});
};
