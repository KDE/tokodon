// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstractaccount.h"

class MockAccount : public AbstractAccount
{
    Q_OBJECT
public:
    MockAccount(QObject *parent = nullptr);
    virtual void get(const QUrl &url, bool authenticated, std::function<void(QNetworkReply *)> callback) override;
    virtual void post(const QUrl &url, const QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)> callback) override;;;;
    virtual void post(const QUrl &url, const QUrlQuery &formdata, bool authenticated, std::function<void(QNetworkReply *)> callback) override;
    virtual void post(const QUrl &url, QHttpMultiPart *message, bool authenticated, std::function<void(QNetworkReply *)> callback) override;
    virtual void put(const QUrl &url, const QJsonDocument &doc, bool authenticated, std::function<void(QNetworkReply *)> callback) override;
    virtual void upload(Post *post, QFile *file, const QString &filename) override;
    virtual void writeToSettings(QSettings &settings) const override;
    virtual void buildFromSettings(const QSettings &settings) override;
    virtual void validateToken() override;
};

