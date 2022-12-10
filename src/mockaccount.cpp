// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "mockaccount.h"

MockAccount::MockAccount(QObject *parent)
    : AbstractAccount(parent)
{}

void MockAccount::get(const QUrl &url, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{
}
void MockAccount::post(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{}
void MockAccount::post(const QUrl &url, const QUrlQuery &formdata, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{}
void MockAccount::post(const QUrl &url, QHttpMultiPart *message, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{}
void MockAccount::put(const QUrl &url, const QJsonDocument &doc, bool authenticated, QObject *parent, std::function<void(QNetworkReply *)> callback)
{}
void MockAccount::upload(Post *post, QFile *file, const QString &filename) 
{}

void MockAccount::writeToSettings(QSettings &settings) const
{}
void MockAccount::buildFromSettings(const QSettings &settings)
{}

void MockAccount::validateToken()
{}
