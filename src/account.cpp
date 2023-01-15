// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "account.h"
#include "accountmanager.h"
#include "notificationhandler.h"
#include "preferences.h"
#include "tokodon_debug.h"
#include "tokodon_http_debug.h"
#include <QNetworkAccessManager>

Account::Account(const QString &name, const QString &instanceUri, QNetworkAccessManager *nam, bool ignoreSslErrors, QObject *parent)
    : AbstractAccount(parent, name, instanceUri)
    , m_ignoreSslErrors(ignoreSslErrors)
    , m_qnam(nam)
{
    m_preferences = new Preferences(this);
    setInstanceUri(instanceUri);

    auto notificationHandler = new NotificationHandler(m_qnam, this);
    connect(this, &Account::notification, notificationHandler, [this, notificationHandler](std::shared_ptr<Notification> notification) {
        notificationHandler->handle(notification, this);
    });
}

Account::Account(const QSettings &settings, QNetworkAccessManager *nam, QObject *parent)
    : AbstractAccount(parent)
    , m_qnam(nam)
{
    m_preferences = new Preferences(this);
    auto notificationHandler = new NotificationHandler(m_qnam, this);
    connect(this, &Account::notification, notificationHandler, [this, notificationHandler](std::shared_ptr<Notification> notification) {
        notificationHandler->handle(notification, this);
    });

    buildFromSettings(settings);
}

Account::~Account()
{
    m_identityCache.clear();
}

void Account::get(const QUrl &url,
                  bool authenticated,
                  QObject *parent,
                  std::function<void(QNetworkReply *)> reply_cb,
                  std::function<void(QNetworkReply *)> errorCallback)
{
    QNetworkRequest request = makeRequest(url, authenticated);
    qCDebug(TOKODON_HTTP) << "GET" << url;

    QNetworkReply *reply = m_qnam->get(request);
    reply->setParent(parent);
    handleReply(reply, reply_cb, errorCallback);
}

void Account::post(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> reply_cb)
{
    auto post_data = doc.toJson();

    QNetworkRequest request = makeRequest(url, authenticated);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    qCDebug(TOKODON_HTTP) << "POST" << url << "[" << post_data << "]";

    auto reply = m_qnam->post(request, post_data);
    reply->setParent(parent);
    handleReply(reply, reply_cb);
}

void Account::put(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> reply_cb)
{
    auto post_data = doc.toJson();

    QNetworkRequest request = makeRequest(url, authenticated);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    qCDebug(TOKODON_HTTP) << "PUT" << url << "[" << post_data << "]";

    QNetworkReply *reply = m_qnam->put(request, post_data);
    reply->setParent(parent);
    handleReply(reply, reply_cb);
}

void Account::post(const QUrl &url, const QUrlQuery &formdata, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> reply_cb)
{
    auto post_data = formdata.toString().toLatin1();

    QNetworkRequest request = makeRequest(url, authenticated);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    qCDebug(TOKODON_HTTP) << "POST" << url << "[" << post_data << "]";

    QNetworkReply *reply = m_qnam->post(request, post_data);
    reply->setParent(parent);
    handleReply(reply, reply_cb);
}

void Account::post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> reply_cb)
{
    QNetworkRequest request = makeRequest(url, authenticated);

    qCDebug(TOKODON_HTTP) << "POST" << url << "(multipart-message)";

    QNetworkReply *reply = m_qnam->post(request, message);
    reply->setParent(parent);
    handleReply(reply, reply_cb);
}

void Account::patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{
    QNetworkRequest request = makeRequest(url, authenticated);
    qCDebug(TOKODON_HTTP) << "PATCH" << url << "(multipart-message)";

    QNetworkReply *reply = m_qnam->sendCustomRequest(request, "PATCH", multiPart);
    reply->setParent(parent);
    handleReply(reply, callback);
}

QNetworkRequest Account::makeRequest(const QUrl &url, bool authenticated) const
{
    QNetworkRequest request(url);

    if (authenticated && haveToken()) {
        const QByteArray bearer = QString("Bearer " + m_token).toLocal8Bit();
        request.setRawHeader("Authorization", bearer);
    }

    return request;
}

void Account::handleReply(QNetworkReply *reply, std::function<void(QNetworkReply *)> reply_cb, std::function<void(QNetworkReply *)> errorCallback) const
{
    connect(reply, &QNetworkReply::finished, [reply, reply_cb, errorCallback]() {
        reply->deleteLater();
        if (200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) && !reply->url().toString().contains("nodeinfo")) {
            qCWarning(TOKODON_LOG) << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) << reply->url();
            if (errorCallback) {
                errorCallback(reply);
            }
            return;
        }
        if (reply_cb) {
            reply_cb(reply);
        }
    });
    if (m_ignoreSslErrors) {
        connect(reply, &QNetworkReply::sslErrors, this, [reply](const QList<QSslError> &) {
            reply->ignoreSslErrors();
        });
    }
}

// assumes file is already opened and named
void Account::upload(Post *p, QFile *file, const QString &filename)
{
    auto mp = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("attachment; name=\"file\"; filename=\"%1\"").arg(filename));
    filePart.setBodyDevice(file);
    file->setParent(mp);

    mp->append(filePart);

    const auto uploadUrl = apiUrl("/api/v1/media");
    qCDebug(TOKODON_HTTP) << "POST" << uploadUrl << "(upload)";

    post(uploadUrl, mp, true, this, [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isObject()) {
            return;
        }

        p->addAttachment(doc.object());
    });
}

static QMap<QString, AbstractAccount::StreamingEventType> stringToStreamingEventType = {
    {"update", AbstractAccount::StreamingEventType::UpdateEvent},
    {"delete", AbstractAccount::StreamingEventType::DeleteEvent},
    {"notification", AbstractAccount::StreamingEventType::NotificationEvent},
    {"filters_changed", AbstractAccount::StreamingEventType::FiltersChangedEvent},
    {"conversation", AbstractAccount::StreamingEventType::ConversationEvent},
    {"announcement", AbstractAccount::StreamingEventType::AnnouncementEvent},
    {"announcement.reaction", AbstractAccount::StreamingEventType::AnnouncementRedactedEvent},
    {"announcement.delete", AbstractAccount::StreamingEventType::AnnouncementDeletedEvent},
    {"status.update", AbstractAccount::StreamingEventType::StatusUpdatedEvent},
    {"encrypted_message", AbstractAccount::StreamingEventType::EncryptedMessageChangedEvent},
};

QWebSocket *Account::streamingSocket(const QString &stream)
{
    if (m_websockets.contains(stream)) {
        return m_websockets[stream];
    }

    auto socket = new QWebSocket();
    socket->setParent(this);

    const auto url = streamingUrl(stream);

    connect(socket, &QWebSocket::textMessageReceived, this, [=](const QString &message) {
        const auto env = QJsonDocument::fromJson(message.toLocal8Bit());

        const auto event = stringToStreamingEventType[env.object()["event"].toString()];
        Q_EMIT streamingEvent(event, env.object()["payload"].toString().toLocal8Bit());

        if (event == NotificationEvent) {
            const auto doc = QJsonDocument::fromJson(env.object()["payload"].toString().toLocal8Bit());
            handleNotification(doc);
            return;
        }
    });

    socket->open(url);

    m_websockets[stream] = socket;
    return socket;
}

void Account::validateToken()
{
    if (!haveToken()) {
        return;
    }

    const QUrl verify_credentials = apiUrl("/api/v1/accounts/verify_credentials");

    get(verify_credentials, true, this, [=](QNetworkReply *reply) {
        if (!reply->isFinished()) {
            return;
        }

        const auto doc = QJsonDocument::fromJson(reply->readAll());

        if (!doc.isObject()) {
            return;
        }

        const auto object = doc.object();
        if (!object.contains("source")) {
            return;
        }

        m_identity = identityLookup(object["id"].toString(), object);
        Q_EMIT identityChanged();
        Q_EMIT authenticated();
    });

    fetchInstanceMetadata();

    // set up streaming for notifications
    streamingSocket("user");
}

void Account::writeToSettings(QSettings &settings) const
{
    settings.beginGroup(m_name);

    settings.setValue("token", m_token);
    settings.setValue("client_id", m_client_id);
    settings.setValue("client_secret", m_client_secret);
    settings.setValue("instance_uri", m_instance_uri);
    settings.setValue("name", m_name);
    settings.setValue("ignoreSslErrors", m_ignoreSslErrors);

    settings.endGroup();
}

void Account::buildFromSettings(const QSettings &settings)
{
    if (!settings.value("token").toString().isEmpty()) {
        m_token = settings.value("token").toString();
        m_client_id = settings.value("client_id").toString();
        m_client_secret = settings.value("client_secret").toString();
        m_name = settings.value("name").toString();
        m_instance_uri = settings.value("instance_uri").toString();
        m_ignoreSslErrors = settings.value("ignoreSslErrors", false).toBool();
        validateToken();
    }
}
