// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstractaccount.h"
#include <QNetworkReply>

class MockAccount : public AbstractAccount
{
    Q_OBJECT
public:
    MockAccount(QObject *parent = nullptr);
    virtual void get(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback, std::function<void(QNetworkReply *)> errorCallback = nullptr) override;
    virtual void post(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;
    virtual void post(const QUrl &url, const QUrlQuery &formdata, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;
    virtual void post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;
    virtual void put(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback) override;
    virtual void upload(Post *post, QFile *file, const QString &filename) override;
    void patch(const QUrl &url, QHttpMultiPart *multiPart, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)>) override;
    virtual void writeToSettings(QSettings &settings) const override;
    virtual void buildFromSettings(const QSettings &settings) override;
    virtual void validateToken() override;

    void registerPost(const QString &url, QNetworkReply *reply);
    void registerGet(const QUrl &url, QNetworkReply *reply);

private:
    QHash<QUrl, QNetworkReply *> m_postReplies;
    QHash<QUrl, QNetworkReply *> m_getReplies;
};
