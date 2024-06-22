// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QtQml>

class AbstractAccount;

class NetworkController : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Controller)
    QML_SINGLETON

    Q_PROPERTY(bool pushNotificationsAvailable READ pushNotificationsAvailable CONSTANT)

public:
    static NetworkController *create(QQmlEngine *, QJSEngine *)
    {
        auto inst = &instance();
        QJSEngine::setObjectOwnership(inst, QJSEngine::ObjectOwnership::CppOwnership);
        return inst;
    }

    static NetworkController &instance();
    Q_INVOKABLE void setApplicationProxy();

    Q_INVOKABLE void openWebApLink(QString input);
    void setAuthCode(QUrl authCode);
    void startComposing(const QString &text);

    bool pushNotificationsAvailable() const;

    Q_INVOKABLE void openLink(const QString &input);

    QString endpoint;

Q_SIGNALS:
    void networkErrorOccurred(const QString &errorString);
    void openPost(QString id);
    void openAccount(QString id);
    void receivedAuthCode(QString authCode);
    void openComposer(const QString &text);

private:
    explicit NetworkController(QObject *parent = nullptr);

    void openLink();

    QUrl m_requestedLink;
    bool m_accountsReady = false;
    QString m_storedComposedText;

    QByteArray m_systemHttpProxy;
    QByteArray m_systemHttpsProxy;
};
