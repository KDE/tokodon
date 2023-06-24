// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later OR GPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

#include <QObject>

class NetworkController : public QObject
{
    Q_OBJECT
public:
    static NetworkController &instance();
    Q_INVOKABLE void setApplicationProxy();

    void openWebApLink(QString url);
    void setAuthCode(QUrl authCode);
    void startComposing(const QString &text);

Q_SIGNALS:
    void networkErrorOccurred(const QString &errorString);
    void openPost(QString id);
    void openAccount(QString id);
    void receivedAuthCode(QString authCode);
    void openComposer(const QString &text);

private:
    explicit NetworkController(QObject *parent = nullptr);

    void openLink();

    QString m_requestedLink;
    bool m_accountsReady = false;
    QString m_storedComposedText;
};
