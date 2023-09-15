// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "accountmanager.h"
#include "account.h"
#include "accountconfig.h"
#include "config.h"
#include "network/networkaccessmanagerfactory.h"
#include "tokodon_debug.h"
#include <qt6keychain/keychain.h>

AccountManager::AccountManager(QObject *parent)
    : QAbstractListModel(parent)
    , m_selected_account(nullptr)
    , m_qnam(NetworkAccessManagerFactory().create(this))
{
    migrateSettings();
    loadFromSettings();

    connect(this, &AccountManager::accountSelected, this, [=](AbstractAccount *account) {
        if (account != nullptr) {
            account->checkForFollowRequests();
        }
    });
}

AccountManager::~AccountManager()
{
    for (auto a : std::as_const(m_accounts)) {
        delete a;
    }

    m_accounts.clear();
}

QVariant AccountManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    auto account = m_accounts.at(index.row());
    if (account->identity() == nullptr) {
        return {};
    }
    switch (role) {
    case Qt::DisplayRole:
    case DisplayNameRole:
        return account->identity()->displayNameHtml();
    case DescriptionRole:
        return account->identity()->account();
    case InstanceRole:
        return account->instanceName();
    case AccountRole:
        return QVariant::fromValue(m_accounts[index.row()]);
    }
    return {};
}

int AccountManager::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return m_accounts.size();
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

AbstractAccount *AccountManager::createNewAccount(const QString &instanceUri, bool ignoreSslErrors, bool admin)
{
    return new Account(instanceUri, m_qnam, ignoreSslErrors, admin, this);
}

bool AccountManager::hasAccounts() const
{
    return !m_accounts.empty();
}

bool AccountManager::hasAnyAccounts() const
{
    return m_hasAnyAccounts;
}

void AccountManager::addAccount(AbstractAccount *account, bool skipAuthenticationCheck)
{
    beginInsertRows(QModelIndex(), m_accounts.size(), m_accounts.size());
    m_accounts.append(account);
    int acctIndex = m_accountStatus.size();
    if (!skipAuthenticationCheck) {
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
    if (!skipAuthenticationCheck) {
        connect(account, &Account::authenticated, this, [this, acctIndex](const bool authenticated, const QString &errorMessage) {
            if (authenticated) {
                m_accountStatus[acctIndex] = AccountStatus::Loaded;
            } else {
                m_accountStatus[acctIndex] = AccountStatus::InvalidCredentials;
                m_accountStatusStrings[acctIndex] = errorMessage;
            }
            Q_EMIT dataChanged(index(0, 0), index(m_accounts.size() - 1, 0));
        });
    }

    connect(account, &Account::fetchedTimeline, this, [this, account](QString original_name, QList<Post *> posts) {
        Q_EMIT fetchedTimeline(account, original_name, posts);
    });
    connect(account, &Account::invalidated, this, [this, account]() {
        Q_EMIT invalidated(account);
    });
    connect(account, &Account::fetchedInstanceMetadata, this, [this, account]() {
        Q_EMIT fetchedInstanceMetadata(account);
        Q_EMIT dataChanged(index(0, 0), index(m_accounts.size() - 1, 0));
    });
    connect(account, &Account::invalidatedPost, this, [this, account](Post *p) {
        Q_EMIT invalidatedPost(account, p);
    });
    connect(account, &Account::notification, this, [this, account](std::shared_ptr<Notification> n) {
        Q_EMIT notification(account, n);
    });

    if (m_selected_account == nullptr) {
        m_selected_account = account;
        Q_EMIT accountSelected(m_selected_account);
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

    auto accessTokenJob = new QKeychain::DeletePasswordJob{"Tokodon"};
    accessTokenJob->setKey(account->accessTokenKey());
    accessTokenJob->start();

    auto clientSecretJob = new QKeychain::DeletePasswordJob{"Tokodon"};
    clientSecretJob->setKey(account->clientSecretKey());
    clientSecretJob->start();

    const auto index = m_accounts.indexOf(account);
    beginRemoveRows(QModelIndex(), index, index);
    m_accounts.removeOne(account);
    endRemoveRows();

    if (hasAccounts()) {
        m_selected_account = m_accounts[0];
    } else {
        m_selected_account = nullptr;
    }
    Q_EMIT accountSelected(m_selected_account);

    Q_EMIT accountRemoved(account);
    Q_EMIT accountsChanged();
}

void AccountManager::reloadAccounts()
{
    for (auto account : std::as_const(m_accounts)) {
        account->validateToken();
    }

    Q_EMIT accountsReloaded();
}

bool AccountManager::selectedAccountHasIssue() const
{
    if (!m_selected_account) {
        return false;
    }

    const int index = m_accounts.indexOf(m_selected_account);
    if (index != -1) {
        return m_accountStatus[index] != AccountStatus::InvalidCredentials;
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
        qDebug() << "WTF: attempt to select unmanaged account" << account;
        return;
    }

    m_selected_account = account;

    if (explicitUserAction) {
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
    return m_selected_account->identity()->id();
}

int AccountManager::selectedIndex() const
{
    for (int i = 0; i < m_accounts.length(); i++) {
        if (m_selected_account == m_accounts[i]) {
            return i;
        }
    }
    return -1;
}

void AccountManager::loadFromSettings()
{
    qCDebug(TOKODON_LOG) << "Loading accounts from settings.";

    auto config = KSharedConfig::openStateConfig();
    for (const auto &id : config->groupList()) {
        if (id.contains('@')) {
            auto accountConfig = new AccountConfig{id};

            int index = m_accountStatus.size();
            m_accountStatus.push_back(AccountStatus::NotLoaded);
            m_accountStatusStrings.push_back({});

            auto account = new Account(accountConfig, m_qnam);
            connect(account, &Account::authenticated, this, [=](const bool successful, const QString &errorMessage) {
                if (successful && account->haveToken() && account->hasName() && account->hasInstanceUrl()) {
                    m_accountStatus[index] = AccountStatus::Loaded;

                    addAccount(account, true);
                } else {
                    m_accountStatus[index] = AccountStatus::InvalidCredentials;
                    m_accountStatusStrings[index] = errorMessage;
                }

                checkIfLoadingFinished();
            });
        }
    }

    checkIfLoadingFinished();
}

KAboutData AccountManager::aboutData() const
{
    return m_aboutData;
}

void AccountManager::setAboutData(const KAboutData &aboutData)
{
    m_aboutData = aboutData;
    Q_EMIT aboutDataChanged();
}

void AccountManager::checkIfLoadingFinished()
{
    // no accounts at all
    if (m_accountStatus.empty()) {
        m_ready = true;
        Q_EMIT accountsReady();
        return;
    }

    bool finished = true;
    for (auto status : m_accountStatus) {
        if (status == AccountStatus::NotLoaded)
            finished = false;
    }

    if (!finished) {
        return;
    }

    qCDebug(TOKODON_LOG) << "Accounts have finished loading.";

    auto config = Config::self();

    for (auto account : m_accounts) {
        // old LastUsedAccount values used to be only username
        const bool isOldVersion = !config->lastUsedAccount().contains(QLatin1Char('@'));
        const bool isEmpty = config->lastUsedAccount().isEmpty() || config->lastUsedAccount() == '@';
        const bool matchesNewFormat = account->settingsGroupName() == config->lastUsedAccount();
        const bool matchesOldFormat = account->username() == config->lastUsedAccount();

        const bool isValid = isEmpty || (isOldVersion ? matchesOldFormat : matchesNewFormat);

        if (isValid) {
            selectAccount(account, false);
            break;
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
    return name + QLatin1Char('@') + QUrl(instanceUri).host();
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
    return QStringLiteral("%1-flatpak-  client-secret").arg(name);
#else
    return QStringLiteral("%1-access-token").arg(name);
#endif
}

void AccountManager::migrateSettings()
{
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
            const QString newName = childName + QLatin1Char('@') + childInstance;
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

            const QString settingsGroupName = childName + QLatin1Char('@') + childInstance;

            AccountConfig config(settingsGroupName);
            config.setClientId(settings.value("client_id").toString());
            config.setInstanceUri(settings.value("instance_uri").toString());
            config.setName(settings.value("name").toString());
            config.setIgnoreSslErrors(settings.value("ignoreSslErrors").toBool());

            config.save();

            auto accessTokenJob = new QKeychain::WritePasswordJob{"Tokodon"};
            accessTokenJob->setKey(AccountManager::accessTokenKey(settingsGroupName));
            accessTokenJob->setTextData(settings.value("token").toString());
            accessTokenJob->start();

            auto clientSecretJob = new QKeychain::WritePasswordJob{"Tokodon"};
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
