// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "network/networkcontroller.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QUrlQuery>

#include <KLocalizedString>

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "config.h"
#include "messagefiltercontainer.h"
#include "tokodon_http_debug.h"
#include "utils/navigation.h"
#include "utils/texthandler.h"

using namespace Qt::Literals::StringLiterals;

NetworkController::NetworkController(QObject *parent)
    : QObject(parent)
{
    // Save the original system-level proxy settings
    m_systemHttpProxy = qgetenv("http_proxy");
    m_systemHttpsProxy = qgetenv("https_proxy");

    // We don't want to reload accounts, as that may inadvertenly call back into this constructor
    // BUG: 492383 is an example of this
    setApplicationProxy(false);

    connect(&AccountManager::instance(), &AccountManager::accountsReady, this, [this] {
        m_accountsReady = true;
        openLink();

        if (!m_storedComposedText.isEmpty()) {
            Q_EMIT Navigation::instance().openComposer(m_storedComposedText);
            m_storedComposedText.clear();
        }
    });
    m_accountsReady = AccountManager::instance().isReady();
}

NetworkController &NetworkController::instance()
{
    static NetworkController _instance;
    return _instance;
}

void NetworkController::setApplicationProxy(const bool reloadAccounts)
{
    Config *cfg = Config::self();
    QNetworkProxy proxy;

    // define network proxy environment variable
    // this is needed for the media backends which don't obey qt's settings
    QByteArray appProxy;
    if (!cfg->proxyUser().isEmpty()) {
        appProxy += QUrl::toPercentEncoding(cfg->proxyUser());
        if (!cfg->proxyPassword().isEmpty()) {
            appProxy += ":" + QUrl::toPercentEncoding(cfg->proxyPassword());
        }
        appProxy += "@";
    }
    appProxy += cfg->proxyHost().toLocal8Bit() + ":" + QByteArray::number(cfg->proxyPort());

    // type match to ProxyType from config.kcfg
    switch (cfg->proxyType()) {
    case 1: // HTTP
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(cfg->proxyHost());
        proxy.setPort(cfg->proxyPort());
        proxy.setUser(cfg->proxyUser());
        proxy.setPassword(cfg->proxyPassword());
        QNetworkProxy::setApplicationProxy(proxy);

        // also set it through environment variables for the media backends
        appProxy.prepend("http://");
        qputenv("http_proxy", appProxy);
        qputenv("https_proxy", appProxy);
        break;
    case 2: // SOCKS 5
        proxy.setType(QNetworkProxy::Socks5Proxy);
        proxy.setHostName(cfg->proxyHost());
        proxy.setPort(cfg->proxyPort());
        proxy.setUser(cfg->proxyUser());
        proxy.setPassword(cfg->proxyPassword());
        QNetworkProxy::setApplicationProxy(proxy);

        // also set it through environment variables for the media backends
        appProxy.prepend("socks5://");
        qputenv("http_proxy", appProxy);
        qputenv("https_proxy", appProxy);
        break;
    case 3:
        proxy.setType(QNetworkProxy::NoProxy);
        QNetworkProxy::setApplicationProxy(proxy);

        // also reset environment variables if they have been set
        qunsetenv("http_proxy");
        qunsetenv("https_proxy");
        break;
    case 0: // System Default
    default:
        QNetworkProxyFactory::setUseSystemConfiguration(true);

        // also reset env variables that might have been overridden
        if (!m_systemHttpProxy.isEmpty()) {
            qputenv("http_proxy", m_systemHttpProxy);
        } else {
            qunsetenv("http_proxy");
        }
        if (!m_systemHttpProxy.isEmpty()) {
            qputenv("https_proxy", m_systemHttpsProxy);
        } else {
            qunsetenv("https_proxy");
        }
        break;
    }

    if (reloadAccounts) {
        AccountManager::instance().reloadAccounts();
    }
}

void NetworkController::openWebApLink(QString input)
{
    QUrl url(input);
    // only web+ap/tokodon (declared in app manifest) and https (explicitly not declared, can be used from command line)
    if (url.scheme() != QStringLiteral("web+ap") && url.scheme() != QStringLiteral("https") && url.scheme() != QStringLiteral("tokodon")) {
        // FIXME maybe warn about unsupported links?
        return;
    }

    m_requestedLink = std::move(url);

    if (m_accountsReady) {
        openLink();
    }
}

void NetworkController::logError(const QString &url, const QString &message)
{
    // URLs can contain sensitive information like access tokens
    const QString sanitizedUrl = QMessageFilterContainer::self()->filter(url);

    qCWarning(TOKODON_HTTP) << sanitizedUrl << message;

    auto config = KSharedConfig::openStateConfig();
    auto networkGroup = config->group(QStringLiteral("Network"));

    const QString messages = networkGroup.readEntry(QStringLiteral("ErrorMessages"), QString());
    QJsonArray messageArray = QJsonDocument::fromJson(messages.toUtf8()).array();
    messageArray.push_front(QJsonObject{{QStringLiteral("url"), sanitizedUrl}, {QStringLiteral("message"), message}});
    // Limit to the last five error messages
    while (messageArray.size() > 5) {
        messageArray.pop_back();
    }
    networkGroup.writeEntry(QStringLiteral("ErrorMessages"), QJsonDocument(messageArray).toJson(QJsonDocument::Compact));

    config->sync();

    Q_EMIT errorMessagesChanged();
}

QJsonArray NetworkController::errorMessages() const
{
    auto config = KSharedConfig::openStateConfig();
    auto networkGroup = config->group(QStringLiteral("Network"));

    const QString messages = networkGroup.readEntry(QStringLiteral("ErrorMessages"), QString());
    return QJsonDocument::fromJson(messages.toUtf8()).array();
}

void NetworkController::clearErrorMessages()
{
    auto config = KSharedConfig::openStateConfig();
    config->deleteGroup(QStringLiteral("Network"));
    config->sync();

    Q_EMIT errorMessagesChanged();
}

bool NetworkController::setAuthCode(QUrl authCode)
{
    QUrlQuery query(authCode);

    if (query.hasQueryItem(QStringLiteral("code"))) {
        Q_EMIT receivedAuthCode(query.queryItemValue(QStringLiteral("code")));
        return true;
    }
    return false;
}

void NetworkController::openLink()
{
    if (m_requestedLink.isEmpty())
        return;

    auto account = AccountManager::instance().selectedAccount();

    if (m_requestedLink.scheme() == QStringLiteral("web+ap") || m_requestedLink.scheme() == QStringLiteral("tokodon")) {
        if (m_requestedLink.userName() == QStringLiteral("tag")) {
            // TODO implement in a future MR
            m_requestedLink.clear();
            return;
        }
        m_requestedLink.setScheme(QStringLiteral("https"));
        m_requestedLink.setUserInfo(QString());
    }

    const QUrl instanceUrl(account->instanceUri());

    // TODO: this assumes the post id is in the last path segment
    // Is this always true? Maybe there's some way to query it.
    if (instanceUrl.host() == m_requestedLink.host()) {
        QString path = m_requestedLink.path();
        path.remove(0, path.lastIndexOf(QLatin1Char('/')) + 1);

        Q_EMIT Navigation::instance().openPost(path);
        return;
    }

    account->requestRemoteObject(m_requestedLink, account, [this](QNetworkReply *reply) {
        const auto searchResult = QJsonDocument::fromJson(reply->readAll()).object();

        const auto statuses = searchResult[QStringLiteral("statuses")].toArray();
        const auto accounts = searchResult[QStringLiteral("accounts")].toArray();

        if (statuses.isEmpty()) {
            qCDebug(TOKODON_HTTP) << "Failed to find any statuses!";
        } else {
            const auto status = statuses[0].toObject();

            Q_EMIT Navigation::instance().openPost(status["id"_L1].toString());
        }

        if (accounts.isEmpty()) {
            qCDebug(TOKODON_HTTP) << "Failed to find any accounts!";
        } else {
            const auto account = accounts[0].toObject();

            Q_EMIT Navigation::instance().openAccount(account["id"_L1].toString());
        }

        m_requestedLink.clear();
    });
}

void NetworkController::startComposing(const QString &text)
{
    if (m_accountsReady) {
        Q_EMIT Navigation::instance().openComposer(text);
    } else {
        m_storedComposedText = text;
    }
}

bool NetworkController::pushNotificationsAvailable() const
{
    return !endpoint.isEmpty();
}

void NetworkController::openLink(const QString &input)
{
    // TODO: expand to profiles?
    if (TextHandler::isPostUrl(input)) {
        auto account = AccountManager::instance().selectedAccount();

        // Then request said URL from our server
        account->requestRemoteObject(QUrl(input), account, [=](QNetworkReply *reply) {
            const auto searchResult = QJsonDocument::fromJson(reply->readAll()).object();

            const auto statuses = searchResult[QStringLiteral("statuses")].toArray();

            if (!statuses.isEmpty()) {
                Navigation::instance().openPost(statuses.last()[QStringLiteral("id")].toString());
            } else {
                // worst case, open it in a web browser
                QDesktopServices::openUrl(QUrl::fromUserInput(input));
            }
        });
    } else {
        QDesktopServices::openUrl(QUrl::fromUserInput(input));
    }
}

void NetworkController::setupPushNotifications()
{
#ifdef HAVE_KUNIFIEDPUSH
    m_connector = new KUnifiedPush::Connector(QStringLiteral("org.kde.tokodon"));
    QObject::connect(m_connector, &KUnifiedPush::Connector::endpointChanged, [this](const auto &newEndpoint) {
        endpoint = newEndpoint;
    });

    endpoint = m_connector->endpoint();

    m_connector->setVapidPublicKeyRequired(true);
    m_connector->registerClient(i18n("Receiving push notifications"));
#endif
}

#ifdef HAVE_KUNIFIEDPUSH
KUnifiedPush::Connector *NetworkController::connector() const
{
    return m_connector;
}
#endif

#include "moc_networkcontroller.cpp"
