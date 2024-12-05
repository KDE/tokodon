// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "account/accountmanager.h"

#include <QJsonDocument>
#include <QNetworkReply>
#include <QSettings>
#include <QUrlQuery>

#include "account/account.h"
#include "config.h"
#include "network/networkaccessmanagerfactory.h"
#include "tokodon_debug.h"

#include <qt6keychain/keychain.h>

using namespace Qt::Literals::StringLiterals;

AccountManager::AccountManager(QObject *parent)
    : QAbstractListModel(parent)
    , m_qnam(NetworkAccessManagerFactory().create(this))
    , m_notificationHandler(new NotificationHandler(m_qnam, this))
{
}

AccountManager::~AccountManager() = default;

QVariant AccountManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    auto account = m_accounts.at(index.row());
    if (!account->identity()) {
        return {};
    }

    switch (role) {
    case Qt::DisplayRole:
    case DisplayNameRole: {
        if (!account->identity()->displayNameHtml().isEmpty()) {
            return account->identity()->displayNameHtml();
        }

        return account->username();
    }
    case DescriptionRole:
        return account->identity()->account();
    case InstanceRole:
        return account->instanceName();
    case AccountRole:
        return QVariant::fromValue(m_accounts[index.row()]);
    default:
        return {};
    }
}

int AccountManager::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return static_cast<int32_t>(m_accounts.size());
}

QHash<int, QByteArray> AccountManager::roleNames() const
{
    return {
        {Qt::DisplayRole, QByteArrayLiteral("display")},
        {DisplayNameRole, QByteArrayLiteral("displayName")},
        {AccountRole, QByteArrayLiteral("account")},
        {DescriptionRole, QByteArrayLiteral("description")},
        {InstanceRole, QByteArrayLiteral("instance")},
    };
}

AccountManager &AccountManager::instance()
{
    static AccountManager accountManager;
    return accountManager;
}

AbstractAccount *AccountManager::createNewAccount(const QString &instanceUri)
{
    return new Account(instanceUri, m_qnam, this);
}

bool AccountManager::hasAccounts() const
{
    return !m_accounts.empty();
}

bool AccountManager::hasAnyAccounts() const
{
    return m_hasAnyAccounts;
}

NotificationHandler *AccountManager::notificationHandler() const
{
    return m_notificationHandler;
}

void AccountManager::addAccount(AbstractAccount *account)
{
    beginInsertRows(QModelIndex(), m_accounts.size(), m_accounts.size());
    m_accounts.append(account);
    int const acctIndex = m_accountStatus.size();
    if (!account->successfullyAuthenticated()) {
        m_accountStatus.push_back(AccountStatus::NotLoaded);
        m_accountStatusStrings.push_back({});
    }
    endInsertRows();

    Q_EMIT accountAdded(account);
    Q_EMIT accountsChanged();
    connect(account, &Account::identityChanged, this, [this, account]() {
        childIdentityChanged(account);
        account->writeToSettings();
    });
    if (!account->successfullyAuthenticated()) {
        connect(account, &Account::authenticated, this, [this, acctIndex](const bool authenticated, const QString &errorMessage) {
            if (authenticated) {
                m_accountStatus[acctIndex] = AccountStatus::Loaded;
            } else {
                m_accountStatus[acctIndex] = AccountStatus::InvalidCredentials;
                m_accountStatusStrings[acctIndex] = errorMessage;
            }
            Q_EMIT dataChanged(index(acctIndex, 0), index(acctIndex, 0));
            checkIfLoadingFinished();
        });
    }
    connect(account, &Account::fetchedInstanceMetadata, this, [this, acctIndex]() {
        Q_EMIT dataChanged(index(acctIndex, 0), index(acctIndex, 0));
    });
    connect(account, &Account::notification, this, [this, account](std::shared_ptr<Notification> n) {
        notificationHandler()->handle(std::move(n), account);
    });

    if (m_selected_account == nullptr) {
        m_selected_account = account;
        Q_EMIT accountSelected(m_selected_account);
    }

    if (m_testMode) {
        checkIfLoadingFinished();
    }
}

void AccountManager::childIdentityChanged(AbstractAccount *account)
{
    Q_EMIT identityChanged(account);

    const auto idx = m_accounts.indexOf(account);
    Q_EMIT dataChanged(index(idx, 0), index(idx, 0));
}

void AccountManager::removeAccount(AbstractAccount *account)
{
    // remove from settings
    auto config = KSharedConfig::openStateConfig();
    config->deleteGroup(account->settingsGroupName());
    config->sync();

    auto accessTokenJob = new QKeychain::DeletePasswordJob{QStringLiteral("Tokodon")};
    accessTokenJob->setKey(account->accessTokenKey());
    accessTokenJob->start();

    auto clientSecretJob = new QKeychain::DeletePasswordJob{QStringLiteral("Tokodon")};
    clientSecretJob->setKey(account->clientSecretKey());
    clientSecretJob->start();

    const auto index = m_accounts.indexOf(account);
    beginRemoveRows(QModelIndex(), index, index);
    m_accounts.removeOne(account);
    endRemoveRows();

    if (hasAccounts()) {
        m_selected_account = m_accounts.first();
    } else {
        m_selected_account = nullptr;
    }
    Q_EMIT accountSelected(m_selected_account);

    Q_EMIT accountRemoved(account);
    Q_EMIT accountsChanged();
}

void AccountManager::reloadAccounts()
{
    bool anyAccountsReloaded = false;
    for (auto account : std::as_const(m_accounts)) {
        if (account->haveToken()) {
            account->validateToken();

            anyAccountsReloaded = true;
        }
    }

    if (anyAccountsReloaded) {
        Q_EMIT accountsReloaded();
    }
}

bool AccountManager::selectedAccountHasIssue() const
{
    if (!m_selected_account) {
        return false;
    }
    return accountHasIssue(m_selected_account);
}

bool AccountManager::accountHasIssue(AbstractAccount *account) const
{
    if (!m_selected_account) {
        return false;
    }

    const int index = m_accounts.indexOf(account);
    if (index != -1 && index < m_accountStatus.size()) {
        return m_accountStatus[index] == AccountStatus::InvalidCredentials;
    }

    return false;
}

QString AccountManager::selectedAccountLoginIssue() const
{
    if (!m_selected_account) {
        return {};
    }

    const int index = m_accounts.indexOf(m_selected_account);
    if (index != -1) {
        return m_accountStatusStrings[index];
    }

    return {};
}

void AccountManager::selectAccount(AbstractAccount *account, bool explicitUserAction)
{
    if (!m_accounts.contains(account)) {
        qCWarning(TOKODON_LOG) << "WTF: attempt to select unmanaged account" << account;
        return;
    }

    m_selected_account = account;

    if (explicitUserAction && !testMode()) {
        auto config = Config::self();
        config->setLastUsedAccount(account->settingsGroupName());
        config->save();
    }

    Q_EMIT accountSelected(account);
}

AbstractAccount *AccountManager::selectedAccount() const
{
    return m_selected_account;
}

QString AccountManager::selectedAccountId() const
{
    if (!m_selected_account) {
        return {};
    }
    return m_selected_account->identity()->id();
}

int AccountManager::selectedIndex() const
{
    return m_accounts.indexOf(m_selected_account);
}

void AccountManager::loadFromSettings()
{
    if (m_testMode) {
        qCDebug(TOKODON_LOG) << "Test mode enabled, no local accounts are loaded.";
        return;
    }

    qCDebug(TOKODON_LOG) << "Loading accounts from settings.";

    auto config = KSharedConfig::openStateConfig();
    for (const auto &id : config->groupList()) {
        if (id.contains('@'_L1)) {
            // The id is normally made up of two parts. <username>@<instance>
            // If one of them is missing, consider the key invalid. Otherwise we may end up with duplicate accounts.
            const QStringList idParts = id.split('@'_L1, Qt::SkipEmptyParts);
            if (idParts.size() != 2) {
                config->deleteGroup(id);
                continue;
            }

            const auto accountConfig = new AccountConfig{id};

            if (accountConfig->clientId().isEmpty() || accountConfig->instanceUri().isEmpty()) {
                config->deleteGroup(id);
                accountConfig->deleteLater();
                continue;
            }

            const auto account = new Account(accountConfig->instanceUri(), m_qnam, this);
            account->setConfig(accountConfig);
            addAccount(account);
        }
    }

    checkIfLoadingFinished();
}

void AccountManager::checkIfLoadingFinished()
{
    // no accounts at all
    if (m_accountStatus.empty()) {
        m_ready = true;
        Q_EMIT accountsReady();
        return;
    }

    // ensure every account is loaded, or has an error
    const bool finished = std::none_of(m_accountStatus.cbegin(), m_accountStatus.cend(), [](const auto status) {
        return status == AccountStatus::NotLoaded;
    });
    if (!finished) {
        return;
    }

    qCDebug(TOKODON_LOG) << "Accounts have finished loading.";

    auto config = Config::self();

    // Test code manually selects the account
    if (!m_testMode) {
        for (auto account : m_accounts) {
            // old LastUsedAccount values used to be only username
            const bool isOldVersion = !config->lastUsedAccount().contains(QLatin1Char('@'));
            const bool isEmpty = config->lastUsedAccount().isEmpty() || config->lastUsedAccount() == '@'_L1;
            const bool matchesNewFormat = account->settingsGroupName() == config->lastUsedAccount();
            const bool matchesOldFormat = account->username() == config->lastUsedAccount();

            const bool isValid = isEmpty || (isOldVersion ? matchesOldFormat : matchesNewFormat);

            if (isValid) {
                selectAccount(account, false);
                break;
            }
        }
    }

    m_ready = true;
    Q_EMIT accountsReady();
}

bool AccountManager::isReady() const
{
    return m_ready;
}

QString AccountManager::settingsGroupName(const QString &name, const QString &instanceUri)
{
    Q_ASSERT(!name.isEmpty());
    Q_ASSERT(!instanceUri.isEmpty());
    return QStringLiteral("%1@%2").arg(name, QUrl::fromUserInput(instanceUri).host());
}

QString AccountManager::clientSecretKey(const QString &name)
{
#ifdef TOKODON_FLATPAK
    return QStringLiteral("%1-flatpak-client-secret").arg(name);
#else
    return QStringLiteral("%1-client-secret").arg(name);
#endif
}

QString AccountManager::accessTokenKey(const QString &name)
{
#ifdef TOKODON_FLATPAK
    return QStringLiteral("%1-flatpak-access-token").arg(name);
#else
    return QStringLiteral("%1-access-token").arg(name);
#endif
}

void AccountManager::migrateSettings()
{
    if (m_testMode) {
        return;
    }

    QSettings settings;

    const auto version = settings.value("settingsVersion", -1).toInt();
    if (version == 0) {
        qCDebug(TOKODON_LOG) << "Migrating v0 settings to v1";
        settings.beginGroup("accounts");
        const auto childGroups = settings.childGroups();
        // we are just going to re-index
        qCDebug(TOKODON_LOG) << "Account list is" << childGroups;
        for (int i = 0; i < childGroups.size(); i++) {
            // we're going to move all of this into an array instead
            const auto child = childGroups[i];
            settings.beginGroup(child);
            const auto keysInChild = settings.childKeys();
            const auto childName = settings.value("name").toString();
            const auto childInstance = QUrl(settings.value("instance_uri").toString()).host();
            const QString newName = QStringLiteral("%1@%2").arg(childName, childInstance);
            qCDebug(TOKODON_LOG) << "Rewriting key from" << child << "to" << newName;
            settings.endGroup();
            for (const auto &key : keysInChild) {
                settings.beginGroup(child);
                const auto value = settings.value(key);
                settings.endGroup(); // child
                settings.beginGroup(newName);
                settings.setValue(key, value);
                settings.endGroup();
            }
            // after porting over the settings, remove it
            settings.remove(child);
        }
        settings.endGroup();
        settings.setValue("settingsVersion", 1);

        // we need to migrate to kconfig
        migrateSettings();
    } else if (version == 1) {
        qCDebug(TOKODON_LOG) << "Migrating v1 settings to kconfig";

        settings.beginGroup("accounts");
        const auto childGroups = settings.childGroups();
        for (int i = 0; i < childGroups.size(); i++) {
            const auto child = childGroups[i];
            settings.beginGroup(child);

            const auto childName = settings.value("name").toString();
            const auto childInstance = QUrl(settings.value("instance_uri").toString()).host();

            const QString settingsGroupName = QStringLiteral("%1@%2").arg(childName, childInstance);

            AccountConfig config(settingsGroupName);
            config.setClientId(settings.value("client_id").toString());
            config.setInstanceUri(settings.value("instance_uri").toString());
            config.setName(settings.value("name").toString());

            config.save();

            auto accessTokenJob = new QKeychain::WritePasswordJob{QStringLiteral("Tokodon")};
            accessTokenJob->setKey(AccountManager::accessTokenKey(settingsGroupName));
            accessTokenJob->setTextData(settings.value("token").toString());
            accessTokenJob->start();

            auto clientSecretJob = new QKeychain::WritePasswordJob{QStringLiteral("Tokodon")};
            clientSecretJob->setKey(AccountManager::clientSecretKey(settingsGroupName));
            clientSecretJob->setTextData(settings.value("client_secret").toString());
            clientSecretJob->start();

            settings.endGroup();
        }

        settings.endGroup();

        // wipe file
        settings.clear();
    }
}

bool AccountManager::isFlatpak() const
{
#ifdef TOKODON_FLATPAK
    return true;
#else
    return false;
#endif
}

void AccountManager::setTestMode(const bool enabled)
{
    m_testMode = enabled;
}

bool AccountManager::testMode() const
{
    return m_testMode;
}

void AccountManager::queueNotifications()
{
    static qsizetype accountsLeft = m_accounts.size();
    static qsizetype totalNotifications = 0;

    const auto checkIfDone = [this]() {
        // If there's no more accounts left to check, and none of them have notifications then early exit
        if (accountsLeft <= 0 && totalNotifications <= 0) {
            Q_EMIT finishedNotificationQueue();
        }
    };

    for (auto account : m_accounts) {
        QUrl uri = account->apiUrl(QStringLiteral("/api/v1/notifications"));

        QUrlQuery urlQuery(uri);
        urlQuery.addQueryItem(QStringLiteral("limit"), QString::number(10));
        if (!account->config()->lastPushNotification().isEmpty()) {
            urlQuery.addQueryItem(QStringLiteral("since_id"), account->config()->lastPushNotification());
        }
        uri.setQuery(urlQuery);

        account->get(
            uri,
            true,
            this,
            [account, checkIfDone](QNetworkReply *reply) {
                const auto data = reply->readAll();
                const auto doc = QJsonDocument::fromJson(data);

                if (!doc.isArray() || doc.array().isEmpty()) {
                    accountsLeft--;
                    checkIfDone();
                    return;
                }

                // We want to post the notifications in reverse order, because the way Plasma displays them.
                // If we post the newest notification, it would be buried by the older ones.
                auto notifications = doc.array().toVariantList();
                std::reverse(notifications.begin(), notifications.end());

                for (const auto &notification : notifications) {
                    if (notification.canConvert<QJsonObject>()) {
                        std::shared_ptr<Notification> n = std::make_shared<Notification>(account, notification.toJsonObject());
                        Q_EMIT account->notification(n);
                    }
                }

                account->config()->setLastPushNotification(doc.array().first()["id"_L1].toString());
                account->config()->save();

                totalNotifications = doc.array().size();

                accountsLeft--;
                checkIfDone();
            },
            [checkIfDone](QNetworkReply *) {
                accountsLeft--;
                checkIfDone();
            });
    }
}

QList<AbstractAccount *> AccountManager::accounts() const
{
    return m_accounts;
}

#include "moc_accountmanager.cpp"
