// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later OR GPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "networkcontroller.h"

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "config.h"
#include "tokodon_http_debug.h"

#include <QNetworkProxy>
#include <QUrlQuery>

NetworkController::NetworkController(QObject *parent)
    : QObject(parent)
{
    setApplicationProxy();

    connect(&AccountManager::instance(), &AccountManager::accountsReady, this, [=] {
        m_accountsReady = true;
        openLink();
    });
}

NetworkController &NetworkController::instance()
{
    static NetworkController _instance;
    return _instance;
}

void NetworkController::setApplicationProxy()
{
    Config *cfg = Config::self();
    QNetworkProxy proxy;

    // type match to ProxyType from config.kcfg
    switch (cfg->proxyType()) {
    case 1: // HTTP
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(cfg->proxyHost());
        proxy.setPort(cfg->proxyPort());
        proxy.setUser(cfg->proxyUser());
        proxy.setPassword(cfg->proxyPassword());
        break;
    case 2: // SOCKS 5
        proxy.setType(QNetworkProxy::Socks5Proxy);
        proxy.setHostName(cfg->proxyHost());
        proxy.setPort(cfg->proxyPort());
        proxy.setUser(cfg->proxyUser());
        proxy.setPassword(cfg->proxyPassword());
        break;
    case 0: // System Default
    default:
        // do nothing
        break;
    }

    QNetworkProxy::setApplicationProxy(proxy);

    AccountManager::instance().reloadAccounts();
}

void NetworkController::openWebApLink(QString url)
{
    if (url.startsWith("web+ap")) {
        url = url.replace(QRegularExpression("(web\\+ap)+:\\/\\/"), "https://");
    }

    m_requestedLink = url;

    if (m_accountsReady) {
        openLink();
    }
}

void NetworkController::setAuthCode(QUrl authCode)
{
    QUrlQuery query(authCode);

    if (query.hasQueryItem("code")) {
        Q_EMIT receivedAuthCode(query.queryItemValue("code"));
    }
}

void NetworkController::openLink()
{
    if (m_requestedLink.isEmpty())
        return;

    auto account = AccountManager::instance().selectedAccount();

    auto url = account->apiUrl("/api/v2/search");
    url.setQuery({
        {"q", m_requestedLink},
        {"resolve", "true"},
        {"limit", "1"},
    });
    account->get(url, true, &AccountManager::instance(), [=](QNetworkReply *reply) {
        const auto searchResult = QJsonDocument::fromJson(reply->readAll()).object();
        const auto statuses = searchResult[QStringLiteral("statuses")].toArray();
        const auto accounts = searchResult[QStringLiteral("accounts")].toArray();

        if (statuses.isEmpty()) {
            qCDebug(TOKODON_HTTP) << "Failed to find any statuses!";
        } else {
            const auto status = statuses[0].toObject();

            Q_EMIT NetworkController::instance().openPost(status["id"].toString());
        }

        if (accounts.isEmpty()) {
            qCDebug(TOKODON_HTTP) << "Failed to find any accounts!";
        } else {
            const auto account = accounts[0].toObject();

            Q_EMIT NetworkController::instance().openAccount(account["id"].toString());
        }

        m_requestedLink.clear();
    });
}
