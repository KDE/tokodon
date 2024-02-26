// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "autotests/mockaccount.h"
#include "account/notificationhandler.h"
#include "autotests/helperreply.h"

using namespace Qt::Literals::StringLiterals;

MockAccount::MockAccount(QObject *parent)
    : AbstractAccount(parent)
{
    registerGet(apiUrl(QStringLiteral("/api/v1/preferences")), new TestReply(QStringLiteral("preferences.json"), this));
    m_preferences = new Preferences(this);
    auto notificationHandler = new NotificationHandler(new QNetworkAccessManager, this);
    connect(this, &MockAccount::notification, notificationHandler, [this, notificationHandler](std::shared_ptr<Notification> notification) {
        notificationHandler->handle(notification, this);
    });

    Q_EMIT authenticated(true, {});
}

void MockAccount::get(const QUrl &url,
                      bool authenticated,
                      QObject *parent,
                      std::function<void(QNetworkReply *)> callback,
                      std::function<void(QNetworkReply *)> errorCallback)
{
    Q_UNUSED(authenticated)
    Q_UNUSED(parent)
    Q_UNUSED(errorCallback)

    if (m_getReplies.contains(url)) {
        auto reply = m_getReplies[url];
        reply->open(QIODevice::ReadOnly);
        callback(reply);
        reply->seek(0);
    } else {
        qWarning() << url << m_getReplies;
        if (errorCallback)
            errorCallback(nullptr);
    }
}

void MockAccount::post(const QUrl &url,
                       const QJsonDocument &doc,
                       bool authenticated,
                       QObject *parent,
                       std::function<void(QNetworkReply *)> callback,
                       std::function<void(QNetworkReply *)> errorCallback,
                       QHash<QByteArray, QByteArray> headers)
{
    Q_UNUSED(doc)
    Q_UNUSED(authenticated)
    Q_UNUSED(parent)
    Q_UNUSED(errorCallback)
    Q_UNUSED(headers)

    if (m_postReplies.contains(url)) {
        auto reply = m_postReplies[url];
        reply->open(QIODevice::ReadOnly);
        callback(reply);
    }
}

void MockAccount::post(const QUrl &url,
                       const QUrlQuery &formdata,
                       bool authenticated,
                       QObject *parent,
                       std::function<void(QNetworkReply *)> callback,
                       std::function<void(QNetworkReply *)> errorCallback)
{
    Q_UNUSED(url)
    Q_UNUSED(authenticated)
    Q_UNUSED(parent)
    Q_UNUSED(callback)
    Q_UNUSED(formdata)
    Q_UNUSED(errorCallback)
}

QNetworkReply *MockAccount::post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{
    Q_UNUSED(url)
    Q_UNUSED(authenticated)
    Q_UNUSED(parent)
    Q_UNUSED(callback)
    Q_UNUSED(message)
    return nullptr;
}

void MockAccount::put(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{
    Q_UNUSED(url)
    Q_UNUSED(authenticated)
    Q_UNUSED(parent)
    Q_UNUSED(callback)
    Q_UNUSED(doc)
}

void MockAccount::put(const QUrl &url, const QUrlQuery &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{
    Q_UNUSED(url)
    Q_UNUSED(authenticated)
    Q_UNUSED(parent)
    Q_UNUSED(callback)
    Q_UNUSED(doc)
}

void MockAccount::patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{
    Q_UNUSED(url)
    Q_UNUSED(authenticated)
    Q_UNUSED(parent)
    Q_UNUSED(callback)
    Q_UNUSED(multiPart)
}

void MockAccount::deleteResource(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{
    Q_UNUSED(url)
    Q_UNUSED(authenticated)
    Q_UNUSED(parent)
    Q_UNUSED(callback)
}

QNetworkReply *MockAccount::upload(const QUrl &filename, std::function<void(QNetworkReply *)> callback)
{
    Q_UNUSED(callback)
    Q_UNUSED(filename)
    return nullptr;
}

void MockAccount::writeToSettings()
{
}

void MockAccount::buildFromSettings()
{
}

void MockAccount::validateToken(bool newAccount)
{
    Q_UNUSED(newAccount)
}

bool MockAccount::hasFollowRequests() const
{
    return false;
}

void MockAccount::checkForFollowRequests()
{
}

void MockAccount::registerPost(const QString &url, QNetworkReply *reply)
{
    m_postReplies[apiUrl(url)] = reply;
}

void MockAccount::registerGet(const QUrl &url, QNetworkReply *reply)
{
    m_getReplies[url] = reply;
}

void MockAccount::setFakeIdentity(const QJsonObject &object)
{
    m_identity = std::make_shared<Identity>();
    m_identity->fromSourceData(object);
}

void MockAccount::clearFakeIdentity()
{
    m_identity.reset();
}

void MockAccount::mentionNotification()
{
    readNotificationFromFile("notification_mention.json"_L1);
}

void MockAccount::favoriteNotification()
{
    readNotificationFromFile("notification_favorite.json"_L1);
}

void MockAccount::boostNotification()
{
    readNotificationFromFile("notification_boost.json"_L1);
}

void MockAccount::followNotification()
{
    readNotificationFromFile("notification_follow.json"_L1);
}

void MockAccount::followRequestNotification()
{
    readNotificationFromFile("notification_request.json"_L1);
}

void MockAccount::statusNotification()
{
    readNotificationFromFile("notification_status.json"_L1);
}

void MockAccount::updateNotification()
{
    readNotificationFromFile("notification_update.json"_L1);
}

void MockAccount::pollNotification()
{
    readNotificationFromFile("notification_poll.json"_L1);
}

void MockAccount::readNotificationFromFile(QLatin1String filename)
{
    QFile statusExampleApi;
    statusExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + filename);
    statusExampleApi.open(QIODevice::ReadOnly);

    handleNotification(QJsonDocument::fromJson(statusExampleApi.readAll()));
}
