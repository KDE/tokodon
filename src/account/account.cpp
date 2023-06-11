// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "account.h"
#include "accountconfig.h"
#include "accountmanager.h"
#include "network/networkcontroller.h"
#include "notificationhandler.h"
#include "preferences.h"
#include "tokodon_debug.h"
#include "tokodon_http_debug.h"
#include "utils/utils.h"
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <qt5keychain/keychain.h>

Account::Account(const QString &instanceUri, QNetworkAccessManager *nam, bool ignoreSslErrors, QObject *parent)
    : AbstractAccount(parent, instanceUri)
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

Account::Account(const AccountConfig &settings, QNetworkAccessManager *nam, QObject *parent)
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

void Account::post(const QUrl &url,
                   const QJsonDocument &doc,
                   bool authenticated,
                   QObject *parent,
                   std::function<void(QNetworkReply *)> reply_cb,
                   std::function<void(QNetworkReply *)> error_cb,
                   QHash<QByteArray, QByteArray> headers)
{
    auto post_data = doc.toJson();

    QNetworkRequest request = makeRequest(url, authenticated);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    for (const auto [headerKey, headerValue] : asKeyValueRange(headers)) {
        request.setRawHeader(headerKey, headerValue);
    }
    qCDebug(TOKODON_HTTP) << "POST" << url << "[" << post_data << "]";

    auto reply = m_qnam->post(request, post_data);
    reply->setParent(parent);
    handleReply(reply, reply_cb, error_cb);
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

QNetworkReply *Account::post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> reply_cb)
{
    QNetworkRequest request = makeRequest(url, authenticated);

    qCDebug(TOKODON_HTTP) << "POST" << url << "(multipart-message)";

    QNetworkReply *reply = m_qnam->post(request, message);
    reply->setParent(parent);
    handleReply(reply, reply_cb);
	return reply;
}

void Account::patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{
    QNetworkRequest request = makeRequest(url, authenticated);
    qCDebug(TOKODON_HTTP) << "PATCH" << url << "(multipart-message)";

    QNetworkReply *reply = m_qnam->sendCustomRequest(request, "PATCH", multiPart);
    reply->setParent(parent);
    handleReply(reply, callback);
}

void Account::deleteResource(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{
    QNetworkRequest request = makeRequest(url, authenticated);

    qCDebug(TOKODON_HTTP) << "DELETE" << url << "(multipart-message)";

    QNetworkReply *reply = m_qnam->deleteResource(request);
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
            } else {
                Q_EMIT NetworkController::instance().networkErrorOccurred(reply->errorString());
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
QNetworkReply *Account::upload(const QUrl &filename, std::function<void(QNetworkReply *)> callback)
{
    auto file = new QFile(filename.toLocalFile());
    const QFileInfo info(filename.toLocalFile());
    file->open(QFile::ReadOnly);

    auto mp = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("attachment; name=\"file\"; filename=\"%1\"").arg(info.fileName()));
    filePart.setBodyDevice(file);
    file->setParent(mp);

    mp->append(filePart);

    const auto uploadUrl = apiUrl("/api/v1/media");
    qCDebug(TOKODON_HTTP) << "POST" << uploadUrl << "(upload)";

    return post(uploadUrl, mp, true, this, callback);
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

    get(
        verify_credentials,
        true,
        this,
        [=](QNetworkReply *reply) {
            qDebug() << "got reply:" << reply->url();

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

            qWarning() << "Authenticatied!";

            m_identity = identityLookup(object["id"].toString(), object);
            m_name = m_identity->username();
            Q_EMIT identityChanged();
            Q_EMIT authenticated(true);
        },
        [=](QNetworkReply *reply) {
            Q_EMIT authenticated(false);
        });

    fetchInstanceMetadata();

    // set up streaming for notifications
    streamingSocket("user");
}

void Account::writeToSettings()
{
    // do not write to settings if we do not have complete information yet,
    // or else it writes malformed and possibly duplicate accounts to settings.
    if (m_name.isEmpty() || m_instance_uri.isEmpty()) {
        return;
    }

    qDebug() << "Writing to settings...";

    AccountConfig config(settingsGroupName());
    config.setClientId(m_client_id);
    config.setClientSecret(m_client_secret);
    config.setInstanceUri(m_instance_uri);
    config.setName(m_name);
    config.setIgnoreSslErrors(m_ignoreSslErrors);

    config.save();

    auto job = new QKeychain::WritePasswordJob{"Tokodon", this};
    job->setKey(m_name);
    job->setTextData(m_token);
    job->start();
}

void Account::buildFromSettings(const AccountConfig &settings)
{
    m_client_id = settings.clientId();
    m_client_secret = settings.clientSecret();
    m_name = settings.name();
    m_instance_uri = settings.instanceUri();
    m_ignoreSslErrors = settings.ignoreSslErrors();

    auto job = new QKeychain::ReadPasswordJob{"Tokodon", this};
    job->setKey(m_name);

    QObject::connect(job, &QKeychain::ReadPasswordJob::finished, [this, job]() {
        m_token = job->textData();
        validateToken();
    });

    job->start();
}

bool Account::hasFollowRequests() const
{
    return m_hasFollowRequests;
}

void Account::checkForFollowRequests()
{
    get(apiUrl("/api/v1/follow_requests"), true, this, [this](QNetworkReply *reply) {
        const auto followRequestResult = QJsonDocument::fromJson(reply->readAll());
        const bool hasFollowRequests = followRequestResult.isArray() && !followRequestResult.array().isEmpty();
        if (hasFollowRequests != m_hasFollowRequests) {
            m_hasFollowRequests = hasFollowRequests;
            Q_EMIT hasFollowRequestsChanged();
        }
    });
}