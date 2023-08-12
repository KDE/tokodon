// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "admin/adminaccountinfo.h"
#include "admin/federationinfo.h"
#include "identity.h"
#include "preferences.h"
#include "timeline/post.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>

class CustomEmoji
{
    Q_GADGET

    Q_PROPERTY(QString shortName MEMBER shortcode)
    Q_PROPERTY(QString unicode MEMBER url)
    Q_PROPERTY(bool isCustom MEMBER isCustom)

public:
    QString shortcode;
    QString url;
    bool isCustom = true;
};

Q_DECLARE_METATYPE(CustomEmoji)

class Attachment;
class Notification;
class QNetworkReply;
class QHttpMultiPart;
class QFile;
class Preferences;
class AccountConfig;

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

public:
    AbstractAccount(QObject *parent, const QString &instanceUri);
    AbstractAccount(QObject *parent);

    /// Register the application to the mastodon server
    void registerApplication(const QString &appName, const QString &website, const QString &additionalScopes = {});

    /// Check if the application is registered
    /// \see registerApplication
    bool isRegistered() const;

    /// Get the oauth2 authorization url
    Q_INVOKABLE QUrl getAuthorizeUrl() const;

    /// Get the oauth2 token url
    QUrl getTokenUrl() const;

    /// Set the oauth2 token
    Q_INVOKABLE void setToken(const QString &authcode);

    /// Check if the account has a token set
    /// \see setToken
    bool haveToken() const;

    /// Check if the account has a username yet
    bool hasName() const;

    /// Check if the account has an instance uri set
    bool hasInstanceUrl() const;

    /// Verifies the token with the instance and if successful, loads identity information for the account
    virtual void validateToken() = 0;

    /// Returns the server-side preferences
    Preferences *preferences() const;

    /// Return the username of the account
    /// \see setUsername
    QString username() const;

    /// Sets the username for the account
    void setUsername(const QString &name);

    /// Fetches instance-specific metadata like max post length, allowed content types, etc
    void fetchInstanceMetadata();

    /// Fetches instance-specific custom emojis
    void fetchCustomEmojis();

    QList<CustomEmoji> customEmojis() const;

    /// Returns the instance URI
    /// \see setInstanceUri
    QString instanceUri() const;

    /// Sets the instance URI for the account
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
    std::shared_ptr<Identity> identityLookup(const QString &accountId, const QJsonObject &doc);

    /// Checks if the accountId exists in the account's identity cache
    bool identityCached(const QString &accountId) const;

    /// Get identity of the admin::account
    std::shared_ptr<AdminAccountInfo> adminIdentityLookup(const QString &accountId, const QJsonObject &doc);

    /// Invalidates the account
    void invalidate();

    /// Favorite a post
    /// \see unfavorite
    void favorite(Post *p);

    /// Unfavorite a post
    /// \see favorite
    void unfavorite(Post *p);

    /// Boost (also known as reblog, or repeat) a post
    /// \see unrepeat
    void repeat(Post *p);

    /// Unboost a post
    /// \see repeat
    void unrepeat(Post *p);

    /// Bookmark a post
    /// \see unbookmark
    void bookmark(Post *p);

    /// Unbookmark a post
    /// \see bookmark
    void unbookmark(Post *p);

    /// Pin a post
    /// \see unpin
    void pin(Post *p);

    /// Unpin a post
    /// \see pin
    void unpin(Post *p);

    /// Returns a streaming url for \p stream
    QUrl streamingUrl(const QString &stream);

    /// Invalidates a post
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
    QUrl apiUrl(const QString &path) const;

    /// Make an HTTP GET request to the mastodon server
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

    /// Make an HTTP POST request to the mastodon server
    /// \param url The url of the request
    /// \param doc The request body as JSON
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    virtual void post(const QUrl &url,
                      const QJsonDocument &doc,
                      bool authenticated,
                      QObject *parent,
                      std::function<void(QNetworkReply *)> callback,
                      std::function<void(QNetworkReply *)> errorCallback = nullptr,
                      QHash<QByteArray, QByteArray> headers = {}) = 0;

    /// Make an HTTP POST request to the mastodon server
    /// \param url The url of the request
    /// \param doc The request body as form-data
    /// \param authenticated Whether the request should be authentificated
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    virtual void post(const QUrl &url, const QUrlQuery &formdata, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    virtual QNetworkReply *post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;
    virtual void put(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;
    virtual void patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)>) = 0;
    virtual void deleteResource(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /// Upload a file
    virtual QNetworkReply *upload(const QUrl &filename, std::function<void(QNetworkReply *)> callback) = 0;

    /// Write account to settings
    virtual void writeToSettings() = 0;

    /// Read account from settings
    virtual void buildFromSettings(const AccountConfig &settings) = 0;

    /// Check if the account has any follow requests
    virtual bool hasFollowRequests() const = 0;

    /// Check against the server for any new follow requests
    virtual void checkForFollowRequests() = 0;

    /// Follow the given account. Can also be used to update whether to show reblogs or enable notifications.
    /// @param Identity identity The account to follow
    /// @param bool reblogs Receive this account's reblogs in home timeline? Defaults to true.
    /// @param bool notify Receive notifications when this account posts a status? Defaults to false.
    Q_INVOKABLE void followAccount(Identity *identity, bool reblogs = true, bool notify = false);

    /// Unfollow the given account.
    /// @param Identity identity The account to unfollow
    Q_INVOKABLE void unfollowAccount(Identity *identity);

    /// Block the given account.
    /// @param Identity identity The account to block
    Q_INVOKABLE void blockAccount(Identity *identity);

    /// Unblock the given account.
    /// @param Identity identity The account to unblock
    Q_INVOKABLE void unblockAccount(Identity *identity);

    /// Mute the given account.
    /// @param Identity identity The account to mute
    /// @param bool notifications Whether notifications should also be muted, by default true
    /// @param int duration How long the mute should last, in seconds. Defaults to 0 (indefinite).
    Q_INVOKABLE void muteAccount(Identity *identity, bool notifications = true, int duration = 0);

    /// Unmute the given account.
    /// @param Identity identity The account to unmute
    Q_INVOKABLE void unmuteAccount(Identity *identity);

    /// Add the given account to the user's featured profiles.
    /// @param Identity identity The account to feature
    Q_INVOKABLE void featureAccount(Identity *identity);

    /// Remove the given account from the user's featured profiles.
    /// @param Identity identity The account to unfeature
    Q_INVOKABLE void unfeatureAccount(Identity *identity);

    /// Sets a private note on a user.
    /// @param Identity identity The account to annotate
    /// @param QString note The note to add to the account. Leave empty to remove the existing note.
    Q_INVOKABLE void addNote(Identity *identity, const QString &note);

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
    /// \see validateToken
    void authenticated(bool successful);

    /// Emitted when the application is successfully registered to the server
    /// \see registerApplication
    void registered();

    /// Emitted when the account's own identity has been updated
    void identityChanged();

    /// Emitted when the requested timeline has been fetched
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
    /// \see invalidatePost
    void invalidatedPost(Post *p);

    /// Emitted when a notification has been received
    void notification(std::shared_ptr<Notification> n);

    /// Emitted when an error occurred when performing an API request
    void errorOccured(const QString &errorMessage);

    /// Emitted when a streaming event has been received
    void streamingEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload);

    /// Emitted when the account has follow requests
    void hasFollowRequestsChanged();

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
    std::shared_ptr<Identity> m_identity;
    std::shared_ptr<AdminAccountInfo> m_adminIdentity;
    AdminAccountInfo *m_federationIdentity;
    AllowedContentType m_allowedContentTypes;
    Preferences *m_preferences = nullptr;
    QList<CustomEmoji> m_customEmojis;
    QString m_additionalScopes;

    // OAuth authorization
    QUrlQuery buildOAuthQuery() const;

    // updates and notifications
    void handleUpdate(const QJsonDocument &doc, const QString &target);
    void handleNotification(const QJsonDocument &doc);

    void mutatePost(Post *p, const QString &verb, bool deliver_home = false);
    QMap<QString, std::shared_ptr<Identity>> m_identityCache;
    QMap<QString, std::shared_ptr<AdminAccountInfo>> m_adminIdentityCache;

    void executeAction(Identity *i, AccountAction accountAction, const QJsonObject &extraArguments = {});
};
