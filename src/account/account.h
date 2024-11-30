// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "account/abstractaccount.h"
#include "account/relationship.h"

#include <QWebSocket>

class AccountConfig;

class Account : public AbstractAccount
{
    Q_OBJECT

public:
    explicit Account(const QString &instanceUri, QNetworkAccessManager *nam, QObject *parent = nullptr);
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
    void put(const QUrl &url, const QUrlQuery &formdata, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;
    void patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)>) override;
    void deleteResource(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;
    QNetworkReply *upload(const QUrl &filename, std::function<void(QNetworkReply *)> callback) override;
    void requestRemoteObject(const QUrl &url, QObject *parent, std::function<void(QNetworkReply *)> callback) override;

    QWebSocket *streamingSocket(const QString &stream);
    QNetworkAccessManager *qnam()
    {
        return m_qnam;
    }

    void writeToSettings() override;

    void buildFromSettings() override;

    void validateToken(bool newAccount = false) override;

    void setConfig(AccountConfig *config);

    Q_INVOKABLE void checkForFollowRequests() override;

    Q_INVOKABLE void checkForUnreadNotifications() override;

    Q_INVOKABLE void updatePushNotifications() override;

    /**
     * @param useAuthCode Whether to use a manually typed code. Otherwise, uses the tokodon:// URI as a callback.
     * @param addAdminScope Whether to request the admin scope.
     */
    Q_INVOKABLE void registerTokodon(bool useAuthCode, bool addAdminScope);

private:
    void unsubscribePushNotifications();
    void subscribePushNotifications();
    QUrlQuery buildNotificationFormData();

    QNetworkAccessManager *m_qnam;
    QMap<QString, QWebSocket *> m_websockets;
    bool m_hasPushSubscription = false;

    // common parts for all HTTP request
    [[nodiscard]] QNetworkRequest makeRequest(const QUrl &url, bool authenticated) const;
    void handleReply(QNetworkReply *reply, std::function<void(QNetworkReply *)> reply_cb, std::function<void(QNetworkReply *)> errorCallback = nullptr) const;
};
