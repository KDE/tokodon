// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QObject>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QImage>
#include <QMap>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QFile>
#include <QWebSocket>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "post.h"

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
    void fromSourceData(QJsonObject doc);
    void fetchAvatar(QUrl &avatar_url);
    void reparentIdentity(Account *parent);

    QNetworkAccessManager *m_qnam;
    Account *m_parent;
};

class Account : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY nameChanged)

    Q_PROPERTY(QString instanceUri READ instanceUri CONSTANT)
    Q_PROPERTY(int maxPostLength READ maxPostLength NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QString instanceName READ instanceName NOTIFY fetchedInstanceMetadata)
    Q_PROPERTY(QUrl authorizeUrl READ getAuthorizeUrl NOTIFY registered)

public:
    Account(QString username, QString instance_uri, QObject *parent = nullptr);
    Account(QSettings &settings, QObject *parent = nullptr);
    ~Account();

    // API stuff (registering)
    void registerApplication();
    bool isRegistered() const;

    // making API calls
    QUrl apiUrl(QString path);
    void get(QUrl url, bool authenticated, std::function<void(QNetworkReply *)>);
    void post(QUrl url, QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)>);
    void post(QUrl url, QUrlQuery &formdata, bool authenticated, std::function<void(QNetworkReply *)>);
    void post(QUrl url, QHttpMultiPart *message, bool authenticated, std::function<void(QNetworkReply *)>);
    void patch(QUrl url, QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)>);
    void put(QUrl url, QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)>);

    // OAuth authorization
    Q_INVOKABLE QUrl getAuthorizeUrl();
    QUrl getTokenUrl();
    Q_INVOKABLE void setToken(const QString &authcode);
    bool haveToken() { return ! m_token.isEmpty (); }
    void validateToken();

    Q_INVOKABLE Post *newPost();

    // name
    QString username() const { return m_name; }
    void setUsername(const QString &name) { m_name = name; }

    // instance metadata
    void fetchInstanceMetadata();
    QString instanceUri() const;
    void setInstanceUri(const QString &instance_uri);
    size_t maxPostLength() const { return m_max_post_length; }
    QString instanceName() const { return QString(m_instance_name); }

    // save/restore.
    // writeToSettings assumes a settings object in a parent context
    // buildFromSettings assumes a settings object in the object context
    void writeToSettings(QSettings &settings);
    void buildFromSettings(QSettings &settings);

    // identity
    const Identity &identity() { return m_identity; }
    void setDirtyIdentity();
    const std::shared_ptr<Identity> identityLookup(const QString &acct, const QJsonObject &doc);
    QNetworkAccessManager *qnam() { return m_qnam; }
    bool identityCached(const QString &acct) const;

    // timeline
    void fetchTimeline(const QString &timeline_name, const QString &from_id);
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
    void fetchAccount(int id, bool excludeReplies,
            std::function<void(QList<std::shared_ptr<Post>>)> final_cb);

    // streaming
    QUrl streamingUrl(QString stream);
    QWebSocket *streamingSocket(QString stream);

    // post refresh
    void invalidatePost(Post *p);

    // Types of formatting that we may use is determined primarily by the server metadata, this is a simple enough
    // way to determine what formats are accepted.
    enum AllowedContentType {
        PlainText = 1 << 0,
        Markdown = 1 << 1,
        Html = 1 << 2,
        BBCode = 1 << 3
    };

    AllowedContentType allowedContentTypes () const {
        return m_allowed_content_types;
    }

Q_SIGNALS:
    void authenticated();
    void registered();
    void identityChanged(Account *);
    void fetchedTimeline(QString, QList<std::shared_ptr<Post>>);
    void invalidated();
    void nameChanged();
    void fetchedInstanceMetadata();
    void invalidatedPost(Post *p);
    void attachmentUploaded(Post *p, Attachment *att);
    void notification(std::shared_ptr<Notification> n);

private:
    QString m_name;
    QString m_instance_uri;
    QString m_token;
    QString m_client_id;
    QString m_client_secret;
    QNetworkAccessManager *m_qnam;
    size_t m_max_post_length;
    QString m_instance_name;
    Identity m_identity;
    AllowedContentType m_allowed_content_types;
    QMap<QString, QWebSocket *> m_websockets;

    QMap<QString, std::shared_ptr<Identity>> m_identity_cache;

    // OAuth authorization
    QUrlQuery buildOAuthQuery();
    void mutatePost(std::shared_ptr<Post> p, const QString verb, bool deliver_home = false);

    // updates and notifications
    void handleUpdate(QJsonDocument doc, QString target);
    void handleNotification(QJsonDocument doc);
};
