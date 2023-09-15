// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "mockaccount.h"
#include "autotests/helperreply.h"

MockAccount::MockAccount(QObject *parent)
    : AbstractAccount(parent)
{
    registerGet(apiUrl("/api/v1/preferences"), new TestReply("preferences.json", this));
    m_preferences = new Preferences(this);

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
    } else {
        qWarning() << url << m_getReplies;
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

void MockAccount::validateToken()
{
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
