// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "networkcontroller.h"

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "config.h"
#include "tokodon_http_debug.h"

#include <QNetworkProxy>
#include <QUrlQuery>

using namespace Qt::Literals::StringLiterals;

NetworkController::NetworkController(QObject *parent)
    : QObject(parent)
{
    setApplicationProxy();

    connect(&AccountManager::instance(), &AccountManager::accountsReady, this, [=] {
        m_accountsReady = true;
        openLink();

        if (!m_storedComposedText.isEmpty()) {
            Q_EMIT openComposer(m_storedComposedText);
            m_storedComposedText.clear();
        }
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
    if (url.startsWith("web+ap"_L1)) {
        url = url.replace(QRegularExpression(QStringLiteral("(web\\+ap)+:\\/\\/")), QStringLiteral("https://"));
    }

    m_requestedLink = url;

    if (m_accountsReady) {
        openLink();
    }
}

void NetworkController::setAuthCode(QUrl authCode)
{
    QUrlQuery query(authCode);

    if (query.hasQueryItem(QStringLiteral("code"))) {
        Q_EMIT receivedAuthCode(query.queryItemValue(QStringLiteral("code")));
    }
}

void NetworkController::openLink()
{
    if (m_requestedLink.isEmpty())
        return;

    auto account = AccountManager::instance().selectedAccount();

    auto url = account->apiUrl(QStringLiteral("/api/v2/search"));
    url.setQuery({
        {QStringLiteral("q"), m_requestedLink},
        {QStringLiteral("resolve"), QStringLiteral("true")},
        {QStringLiteral("limit"), QStringLiteral("1")},
    });
    account->get(url, true, &AccountManager::instance(), [=](QNetworkReply *reply) {
        const auto searchResult = QJsonDocument::fromJson(reply->readAll()).object();
        const auto statuses = searchResult[QStringLiteral("statuses")].toArray();
        const auto accounts = searchResult[QStringLiteral("accounts")].toArray();

        if (statuses.isEmpty()) {
            qCDebug(TOKODON_HTTP) << "Failed to find any statuses!";
        } else {
            const auto status = statuses[0].toObject();

            Q_EMIT NetworkController::instance().openPost(status["id"_L1].toString());
        }

        if (accounts.isEmpty()) {
            qCDebug(TOKODON_HTTP) << "Failed to find any accounts!";
        } else {
            const auto account = accounts[0].toObject();

            Q_EMIT NetworkController::instance().openAccount(account["id"_L1].toString());
        }

        m_requestedLink.clear();
    });
}

void NetworkController::startComposing(const QString &text)
{
    if (m_accountsReady) {
        Q_EMIT openComposer(text);
    } else {
        m_storedComposedText = text;
    }
}
