// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "account/abstractaccount.h"

class MockAccount : public AbstractAccount
{
    Q_OBJECT

public:
    explicit MockAccount(QObject *parent = nullptr);

    bool successfullyAuthenticated() const override;

    void get(const QUrl &url,
             bool authenticated,
             QObject *parent,
             std::function<void(QNetworkReply *)> callback,
             std::function<void(QNetworkReply *)> errorCallback = nullptr,
             bool fallible = false) override;

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
              std::function<void(QNetworkReply *)> errorCallback = nullptr) override;

    QNetworkReply *post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;

    void put(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;
    void put(const QUrl &url, const QUrlQuery &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;

    QNetworkReply *upload(const QUrl &filename, std::function<void(QNetworkReply *)> callback) override;

    void requestRemoteObject(const QUrl &url, QObject *parent, std::function<void(QNetworkReply *)> callback) override;

    void patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)>) override;

    void deleteResource(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;

    void writeToSettings() override;

    void buildFromSettings() override;

    void validateToken() override;

    void checkForFollowRequests() override;

    void checkForUnreadNotifications() override;

    void registerPost(const QString &url, QNetworkReply *reply);

    void registerGet(const QUrl &url, QNetworkReply *reply);

    void setFakeIdentity(const QJsonObject &object);
    void clearFakeIdentity();

    void updatePushNotifications() override {};

    Q_INVOKABLE void mentionNotification();
    Q_INVOKABLE void favoriteNotification();
    Q_INVOKABLE void boostNotification();
    Q_INVOKABLE void followNotification();
    Q_INVOKABLE void followRequestNotification();
    Q_INVOKABLE void statusNotification();
    Q_INVOKABLE void updateNotification();
    Q_INVOKABLE void pollNotification();
    Q_INVOKABLE void annualReportNotification();
    Q_INVOKABLE void unknownNotification();

    Q_INVOKABLE void increaseFollowRequests();
    Q_INVOKABLE void decreaseFollowRequests();

private:
    void readNotificationFromFile(QLatin1String filename);

    QHash<QUrl, QNetworkReply *> m_postReplies;
    QHash<QUrl, QNetworkReply *> m_getReplies;
    QNetworkReply *m_errorReply;
};
