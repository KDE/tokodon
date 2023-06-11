// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "accountmanager.h"
#include "account.h"
#include "accountconfig.h"
#include "config.h"
#include "network/networkaccessmanagerfactory.h"
#include "tokodon_debug.h"

void migrateSettings(QSettings &settings);

AccountManager::AccountManager(QObject *parent)
    : QAbstractListModel(parent)
    , m_selected_account(nullptr)
    , m_qnam(NetworkAccessManagerFactory().create(this))
{
    QSettings settings;
    migrateSettings(settings);
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

AbstractAccount *AccountManager::createNewAccount(const QString &instanceUri, bool ignoreSslErrors)
{
    return new Account(instanceUri, m_qnam, ignoreSslErrors, this);
}

bool AccountManager::hasAccounts() const
{
    return !m_accounts.empty();
}

void AccountManager::addAccount(AbstractAccount *account)
{
    beginInsertRows(QModelIndex(), m_accounts.size(), m_accounts.size());
    m_accounts.append(account);
    endInsertRows();

    Q_EMIT accountAdded(account);
    Q_EMIT accountsChanged();
    connect(account, &Account::identityChanged, this, [this, account]() {
        childIdentityChanged(account);
        qDebug() << "Identity has changed...";
        account->writeToSettings();
    });
    connect(account, &Account::authenticated, this, [this]() {
        Q_EMIT dataChanged(index(0, 0), index(m_accounts.size() - 1, 0));
    });

    connect(account, &Account::fetchedTimeline, this, [this, account](QString original_name, QList<Post *> posts) {
        Q_EMIT fetchedTimeline(account, original_name, posts);
    });
    connect(account, &Account::invalidated, this, [this, account]() {
        Q_EMIT invalidated(account);
    });
    connect(account, &Account::fetchedInstanceMetadata, this, [this, account]() {
        Q_EMIT fetchedInstanceMetadata(account);
    });
    connect(account, &Account::invalidatedPost, this, [this, account](Post *p) {
        Q_EMIT invalidatedPost(account, p);
    });
    connect(account, &Account::notification, this, [this, account](std::shared_ptr<Notification> n) {
        Q_EMIT notification(account, n);
    });
}

void AccountManager::childIdentityChanged(AbstractAccount *account)
{
    Q_EMIT identityChanged(account);

    const auto idx = m_accounts.indexOf(account);
    Q_EMIT dataChanged(index(idx, 0), index(idx, 0));
}

void AccountManager::removeAccount(AbstractAccount *account)
{
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
    qDebug() << "Loading any accounts from settings.";

    KConfig config{"testrc", KConfig::OpenFlag::NoGlobals};
    for (const auto &id : config.groupList()) {
        auto accountConfig = AccountConfig{id};

        int index = m_accountStatus.size();
        m_accountStatus.push_back(AccountStatus::NotLoaded);

        auto account = new Account(accountConfig, m_qnam);
        connect(account, &Account::authenticated, this, [=](bool successful) {
            if (successful && account->haveToken() && account->hasName() && account->hasInstanceUrl()) {
                m_accountStatus[index] = AccountStatus::Loaded;

                addAccount(account);

                qDebug() << "Loaded from settings:" << account;
            } else {
                m_accountStatus[index] = AccountStatus::InvalidCredentials;

                delete account;
            }

            checkIfLoadingFinished();
        });
    }
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
    bool finished = true;
    for (auto status : m_accountStatus) {
        if (status == AccountStatus::NotLoaded)
            finished = false;
    }

    if (!finished) {
        qDebug() << "Not finished loading all accounts yet!";
        return;
    }

    qDebug() << "Finish loading accounts, now setting default";

    auto config = Config::self();

    for (auto account : m_accounts) {
        // old LastUsedAccount values used to be only username
        const bool isOldVersion = !config->lastUsedAccount().contains(QLatin1Char('@'));
        const bool isEmpty = config->lastUsedAccount().isEmpty() || config->lastUsedAccount() == '@';
        const bool matchesNewFormat = account->settingsGroupName() == config->lastUsedAccount();
        const bool matchesOldFormat = account->username() == config->lastUsedAccount();

        const bool isValid = isEmpty || (isOldVersion ? matchesOldFormat : matchesNewFormat);

        if (selectedAccount() == nullptr && isValid) {
            selectAccount(account, false);
        }
    }

    m_ready = true;
    Q_EMIT accountsReady();
}

bool AccountManager::isReady() const
{
    return m_ready;
}

void migrateSettings(QSettings &settings)
{
    const auto version = settings.value("settingsVersion").toInt();
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
    }
}
