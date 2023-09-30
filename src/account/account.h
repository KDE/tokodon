// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QFile>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QImage>
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

#include "abstractaccount.h"
#include "identity.h"
#include "relationship.h"
#include "timeline/post.h"

class AccountConfig;

class Account : public AbstractAccount
{
    Q_OBJECT

public:
    explicit Account(const QString &instance_uri, QNetworkAccessManager *nam, bool ignoreSslErrors = false, bool admin = true, QObject *parent = nullptr);
    explicit Account(AccountConfig *settings, QNetworkAccessManager *nam, QObject *parent = nullptr);
    ~Account() override;
    void get(const QUrl &url,
             bool authenticated,
             QObject *parent,
             std::function<void(QNetworkReply *)> callback,
             std::function<void(QNetworkReply *)> errorCallback = nullptr) override;
    void post(const QUrl &url,
              const QJsonDocument &doc,
              bool authenticated,
              QObject *parent,
              std::function<void(QNetworkReply *)> callback,
              std::function<void(QNetworkReply *)> errorCallback,
              QHash<QByteArray, QByteArray> headers = {}) override;
    void post(const QUrl &url,
              const QUrlQuery &formdata,
              bool authenticated,
              QObject *parent,
              std::function<void(QNetworkReply *)> callback,
              std::function<void(QNetworkReply *)> errorCallback) override;
    QNetworkReply *post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;
    void put(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;
    void patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)>) override;
    void deleteResource(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;
    QNetworkReply *upload(const QUrl &filename, std::function<void(QNetworkReply *)> callback) override;

    QWebSocket *streamingSocket(const QString &stream);
    QNetworkAccessManager *qnam()
    {
        return m_qnam;
    }

    void writeToSettings() override;

    void buildFromSettings() override;

    void validateToken() override;

    bool hasFollowRequests() const override;

    Q_INVOKABLE void checkForFollowRequests() override;

private:
    bool m_ignoreSslErrors = false;
    QNetworkAccessManager *m_qnam;
    QMap<QString, QWebSocket *> m_websockets;

    // common parts for all HTTP request
    QNetworkRequest makeRequest(const QUrl &url, bool authenticated) const;
    void handleReply(QNetworkReply *reply, std::function<void(QNetworkReply *)> reply_cb, std::function<void(QNetworkReply *)> errorCallback = nullptr) const;
};
