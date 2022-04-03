// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QFile>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QSettings>
#include <QUrl>
#include <QUrlQuery>
#include <QWebSocket>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "post.h"
#include "relationship.h"

class Account;

class Identity : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString displayName MEMBER m_display_name CONSTANT)
    Q_PROPERTY(QString bio MEMBER m_bio CONSTANT)
    Q_PROPERTY(QString account MEMBER m_acct CONSTANT)
    Q_PROPERTY(bool locked MEMBER m_locked CONSTANT)
    Q_PROPERTY(QString visibility MEMBER m_visibility CONSTANT)
    Q_PROPERTY(QUrl avatarUrl MEMBER m_avatarUrl CONSTANT)
    Q_PROPERTY(QUrl backgroundUrl MEMBER m_backgroundUrl CONSTANT)
    Q_PROPERTY(int followersCount MEMBER m_followersCount CONSTANT)
    Q_PROPERTY(int followingCount MEMBER m_followingCount CONSTANT)
    Q_PROPERTY(int statusesCount MEMBER m_statusesCount CONSTANT)
    Q_PROPERTY(QJsonArray fields MEMBER m_fields CONSTANT)

    Q_PROPERTY(Relationship *relationship READ relationship NOTIFY relationshipChanged)

public:
    int m_id;
    QString m_display_name;
    QString m_bio;
    QString m_acct;
    bool m_locked;
    QString m_visibility;
    QUrl m_avatarUrl;
    QUrl m_backgroundUrl;
    QJsonArray m_fields;
    int m_followersCount;
    int m_followingCount;
    int m_statusesCount;
    void fromSourceData(const QJsonObject &doc);
    void fetchAvatar(const QUrl &avatar_url);
    void reparentIdentity(Account *parent);

    QNetworkAccessManager *m_qnam;
    Account *m_parent;

    Relationship* relationship() const;
    void setRelationship(Relationship *r);

Q_SIGNALS:
    void relationshipChanged();

private:
    Relationship *m_relationship = nullptr;
};

class Account : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY nameChanged)

    Q_PROPERTY(QString instanceUri READ instanceUri CONSTANT)
    Q_PROPERTY(int maxPostLength READ maxPostLength NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QString instanceName READ instanceName NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QUrl authorizeUrl READ getAuthorizeUrl NOTIFY registered)
    Q_PROPERTY(Identity *identity READ identityObj CONSTANT)

public:
    explicit Account(const QString &username, const QString &instance_uri, QObject *parent = nullptr);
    explicit Account(const QSettings &settings, QObject *parent = nullptr);
    ~Account();

    // API stuff (registering)
    void registerApplication();
    bool isRegistered() const;

    // making API calls
    QUrl apiUrl(const QString &path) const;
    void get(const QUrl &url, bool authenticated, std::function<void(QNetworkReply *)>);
    void post(const QUrl &url, const QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)>);
    void post(const QUrl &url, const QUrlQuery &formdata, bool authenticated, std::function<void(QNetworkReply *)>);
    void post(const QUrl &url, QHttpMultiPart *message, bool authenticated, std::function<void(QNetworkReply *)>);
    void patch(const QUrl &url, const QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)>);
    void put(const QUrl &url, const QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)>);

    // OAuth authorization
    Q_INVOKABLE QUrl getAuthorizeUrl() const;
    QUrl getTokenUrl() const;
    Q_INVOKABLE void setToken(const QString &authcode);
    bool haveToken() const
    {
        return !m_token.isEmpty();
    }
    void validateToken();

    Q_INVOKABLE Post *newPost();

    // name
    QString username() const
    {
        return m_name;
    }
    void setUsername(const QString &name)
    {
        m_name = name;
    }

    // instance metadata
    void fetchInstanceMetadata();
    QString instanceUri() const;
    void setInstanceUri(const QString &instance_uri);
    size_t maxPostLength() const
    {
        return m_maxPostLength;
    }
    QString instanceName() const
    {
        return QString(m_instance_name);
    }

    // save/restore.
    // writeToSettings assumes a settings object in a parent context
    // buildFromSettings assumes a settings object in the object context
    void writeToSettings(QSettings &settings) const;
    void buildFromSettings(const QSettings &settings);

    // identity
    const Identity &identity()
    {
        return m_identity;
    }

    Identity *identityObj()
    {
        return &m_identity;
    }
    void setDirtyIdentity();
    const std::shared_ptr<Identity> identityLookup(const QString &acct, const QJsonObject &doc);
    QNetworkAccessManager *qnam()
    {
        return m_qnam;
    }
    bool identityCached(const QString &acct) const;

    // timeline
    void fetchTimeline(const QString &timelineName, const QString &from_id);
    void invalidate();

    // posting statuses
    Q_INVOKABLE void postStatus(Post *p);
    void favorite(std::shared_ptr<Post> p);
    void unfavorite(std::shared_ptr<Post> p);
    void repeat(std::shared_ptr<Post> p);
    void unrepeat(std::shared_ptr<Post> p);

    // uploading media
    void upload(Post *p, QFile *file, QString filename);
    void updateAttachment(Attachment *a);

    /// Thread fetching
    void fetchThread(const QString &postId, std::function<void(QList<std::shared_ptr<Post>>)> final_cb);

    /// Fetch account timeline
    void fetchAccount(int id, bool excludeReplies, std::function<void(QList<std::shared_ptr<Post>>)> final_cb);

    // streaming
    QUrl streamingUrl(const QString &stream);
    QWebSocket *streamingSocket(const QString &stream);

    // post refresh
    void invalidatePost(Post *p);

    // Types of formatting that we may use is determined primarily by the server metadata, this is a simple enough
    // way to determine what formats are accepted.
    enum AllowedContentType { PlainText = 1 << 0, Markdown = 1 << 1, Html = 1 << 2, BBCode = 1 << 3 };

    AllowedContentType allowedContentTypes() const
    {
        return m_allowedContentTypes;
    }

    // Follow Account
    void followAccount(Identity *i);
    void unfollowAccount(Identity *i);

Q_SIGNALS:
    void authenticated();
    void registered();
    void identityChanged(Account *);
    void fetchedTimeline(const QString &, QList<std::shared_ptr<Post>>);
    void invalidated();
    void nameChanged();
    void fetchedInstanceMetadata();
    void invalidatedPost(Post *p);
    void notification(std::shared_ptr<Notification> n);
    void followRequestBlocked();
    void errorOccured(QString &);

private:
    QString m_name;
    QString m_instance_uri;
    QString m_token;
    QString m_client_id;
    QString m_client_secret;
    QNetworkAccessManager *m_qnam;
    size_t m_maxPostLength;
    QString m_instance_name;
    Identity m_identity;
    AllowedContentType m_allowedContentTypes;
    QMap<QString, QWebSocket *> m_websockets;

    QMap<QString, std::shared_ptr<Identity>> m_identityCache;

    // OAuth authorization
    QUrlQuery buildOAuthQuery() const;
    void mutatePost(std::shared_ptr<Post> p, const QString &verb, bool deliver_home = false);

    // updates and notifications
    void handleUpdate(const QJsonDocument &doc, const QString &target);
    void handleNotification(const QJsonDocument &doc);
};
