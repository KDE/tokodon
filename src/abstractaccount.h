// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "identity.h"
#include "post.h"
#include "preferences.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>

class Attachment;
class Notification;
class QNetworkReply;
class QHttpMultiPart;
class QFile;
class Preferences;

class AbstractAccount : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY nameChanged)

    Q_PROPERTY(QString instanceUri READ instanceUri CONSTANT)
    Q_PROPERTY(int maxPostLength READ maxPostLength NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QString instanceName READ instanceName NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QUrl authorizeUrl READ getAuthorizeUrl NOTIFY registered)
    Q_PROPERTY(Identity *identity READ identityObj CONSTANT)
    Q_PROPERTY(Preferences *preferences READ preferences CONSTANT)

public:
    AbstractAccount(QObject *parent, const QString &name, const QString &instanceUri);
    AbstractAccount(QObject *parent);

    /// Register the application to the mastodon server
    void registerApplication(const QString &appName, const QString &website);

    /// Check if the application is registered
    /// \see registerApplication
    bool isRegistered() const;

    /// Get the oauth2 authorization url
    Q_INVOKABLE QUrl getAuthorizeUrl() const;
    QUrl getTokenUrl() const;
    Q_INVOKABLE void setToken(const QString &authcode);
    Q_INVOKABLE Post *newPost();
    bool haveToken() const;
    virtual void validateToken() = 0;

    Preferences *preferences() const;

    // name
    QString username() const;
    void setUsername(const QString &name);

    // instance metadata
    void fetchInstanceMetadata();
    QString instanceUri() const;
    void setInstanceUri(const QString &instance_uri);
    size_t maxPostLength() const;
    QString instanceName() const;

    // identity
    const Identity &identity();

    Identity *identityObj();
    void setDirtyIdentity();
    const std::shared_ptr<Identity> identityLookup(const QString &acct, const QJsonObject &doc);
    bool identityCached(const QString &acct) const;

    // timeline
    void fetchTimeline(const QString &timelineName, const QString &from_id);
    void invalidate();

    // posting statuses
    Q_INVOKABLE void postStatus(Post *p);
    void favorite(Post *p);
    void unfavorite(Post *p);
    void repeat(Post *p);
    void unrepeat(Post *p);

    // uploading media
    void updateAttachment(Attachment *a);

    // streaming
    QUrl streamingUrl(const QString &stream);

    // post refresh
    void invalidatePost(Post *p);

    // Types of formatting that we may use is determined primarily by the server metadata, this is a simple enough
    // way to determine what formats are accepted.
    enum AllowedContentType { PlainText = 1 << 0, Markdown = 1 << 1, Html = 1 << 2, BBCode = 1 << 3 };

    AllowedContentType allowedContentTypes() const
    {
        return m_allowedContentTypes;
    }

    QUrl apiUrl(const QString &path) const;

    /// Make an HTTP GET request to the mastodon server
    /// \param url The url of the request
    /// \param authenticated Whether the request should be authentificated
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    /// \param errorCallback The callback that should be executed if the request is not successful
    virtual void get(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback, std::function<void(QNetworkReply *)> errorCallback = nullptr) = 0;

    /// Make an HTTP POST request to the mastodon server
    /// \param url The url of the request
    /// \param doc The request body as JSON
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    virtual void post(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    /// Make an HTTP POST request to the mastodon server
    /// \param url The url of the request
    /// \param doc The request body as form-data
    /// \param authenticated Whether the request should be authentificated
    /// \param parent The parent object that calls get() or the callback belongs to
    /// \param callback The callback that should be executed if the request is successful
    virtual void post(const QUrl &url, const QUrlQuery &formdata, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;

    virtual void post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;
    virtual void put(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) = 0;
    virtual void patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)>) = 0;

    /// Upload a file
    virtual void upload(Post *post, QFile *file, const QString &filename) = 0;

    /// Write account to settings
    virtual void writeToSettings(QSettings &settings) const = 0;

    /// Read account from settings
    virtual void buildFromSettings(const QSettings &settings) = 0;

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

    enum AccountAction {
        Follow,
        Unfollow,
        Block,
        Unblock,
        Mute,
        Unmute,
        Feature,
        Unfeature,
        Note,
    };

    enum StreamingEventType {
        UpdateEvent, //< A new Status has appeared.
        DeleteEvent, ///< A status has been deleted.
        NotificationEvent, ///< A new notification has appeared.
        FiltersChangedEvent, ///< Keyword filters have been changed.
        ConversationEvent, ///< A direct conversation has been updated.
        AnnouncementEvent, //< An announcement has been published.
        AnnouncementRedactedEvent, //< An announcement has received an emoji reaction.
        AnnouncementDeletedEvent, //< An announcement has been deleted.
        StatusUpdatedEvent, ///< A Status has been edited.
        EncryptedMessageChangedEvent, ///< An encrypted message has been received.
    };

Q_SIGNALS:
    void authenticated();
    void registered();
    void identityChanged(AbstractAccount *account);
    void fetchedTimeline(const QString &timelineName, QList<Post *> posts);
    void invalidated();
    void nameChanged();
    void usernameChanged();
    void fetchedInstanceMetadata();
    void invalidatedPost(Post *p);
    void notification(std::shared_ptr<Notification> n);
    void followRequestBlocked();
    void errorOccured(const QString &errorMessage);
    void streamingEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload);

protected:
    QString m_name;
    QString m_instance_uri;
    QString m_token;
    QString m_client_id;
    QString m_client_secret;
    size_t m_maxPostLength;
    QString m_instance_name;
    Identity m_identity;
    AllowedContentType m_allowedContentTypes;
    Preferences *m_preferences = nullptr;

    // OAuth authorization
    QUrlQuery buildOAuthQuery() const;

    // updates and notifications
    void handleUpdate(const QJsonDocument &doc, const QString &target);
    void handleNotification(const QJsonDocument &doc);

    void mutatePost(Post *p, const QString &verb, bool deliver_home = false);
    QMap<QString, std::shared_ptr<Identity>> m_identityCache;

    void executeAction(Identity *i, AccountAction accountAction, const QJsonObject &extraArguments = {});
};
