// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "accountmanager.h"

#if HAVE_KACCOUNTS
#include <KAccounts/Core>
#include <Accounts/Manager>
#endif

AccountManager::AccountManager(QObject *parent)
    : QAbstractListModel(parent)
    , m_selected_account(nullptr)
{
    QSettings settings;
    loadFromSettings(settings);
    loadFromKAccounts();
}

AccountManager::~AccountManager()
{
    // before destroying the world, dump everything to QSettings
    QSettings settings;
    writeToSettings(settings);

    for (auto a : m_accounts)
        delete a;

    m_accounts.clear();
}

QVariant AccountManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    switch (role) {
    case Qt::DisplayRole:
        return m_accounts[index.row()]->username();
    case AccountRole:
        return QVariant::fromValue(m_accounts[index.row()]);
    }
    return {};
}

int AccountManager::rowCount(const QModelIndex &index) const
{
    return m_accounts.size();
}

QHash<int, QByteArray> AccountManager::roleNames() const
{
    return {{Qt::DisplayRole, QByteArrayLiteral("display")}, {AccountRole, QByteArrayLiteral("account")}};
}

AccountManager &AccountManager::instance()
{
    static AccountManager accountManager;
    return accountManager;
}

Account *AccountManager::createNewAccount(const QString &username, const QString &instanceUri)
{
    return new Account(username, instanceUri, this);
}

bool AccountManager::hasAccounts() const
{
    return m_accounts.size() > 0;
}

void AccountManager::addAccount(Account *account)
{
    beginInsertRows(QModelIndex(), m_accounts.size(), m_accounts.size());
    m_accounts.append(account);
    endInsertRows();

    Q_EMIT accountAdded(account);
    selectAccount(account);

    QObject::connect(account, &Account::identityChanged, this, &AccountManager::childIdentityChanged);

    QObject::connect(account, &Account::fetchedTimeline, [=](QString original_name, QList<std::shared_ptr<Post>> posts) {
        Q_EMIT fetchedTimeline(account, original_name, posts);
    });
    QObject::connect(account, &Account::invalidated, [=]() {
        Q_EMIT invalidated(account);
    });
    QObject::connect(account, &Account::fetchedInstanceMetadata, [=]() {
        Q_EMIT fetchedInstanceMetadata(account);
    });
    QObject::connect(account, &Account::invalidatedPost, [=](Post *p) {
        Q_EMIT invalidatedPost(account, p);
    });
    QObject::connect(account, &Account::notification, [=](std::shared_ptr<Notification> n) {
        Q_EMIT notification(account, n);
    });
    QSettings settings;
    writeToSettings(settings);
}

void AccountManager::childIdentityChanged(Account *account)
{
    Q_EMIT identityChanged(account);
}

void AccountManager::removeAccount(Account *account)
{
    m_accounts.removeOne(account);

    Q_EMIT accountRemoved(account);
}

void AccountManager::selectAccount(Account *account)
{
    if (!m_accounts.contains(account)) {
        qDebug() << "WTF: attempt to select unmanaged account" << account;
        return;
    }

    m_selected_account = account;

    Q_EMIT accountSelected(account);
}

Account *AccountManager::selectedAccount()
{
    return m_selected_account;
}

void AccountManager::writeToSettings(QSettings &settings)
{
    settings.beginGroup("accounts");

    for (auto a : m_accounts)
        a->writeToSettings(settings);

    settings.endGroup();
}

void AccountManager::loadFromSettings(QSettings &settings)
{
    qDebug() << "Loading any accounts from settings.";

    settings.beginGroup("accounts");

    const auto childGroups = settings.childGroups();
    for (const auto &child : childGroups) {
        settings.beginGroup(child);

        auto account = new Account(settings);
        if (account->haveToken()) {
            addAccount(account);
            qDebug() << "Loaded from settings:" << account;
        } else {
            delete account;
        }

        settings.endGroup();
    }

    settings.endGroup();
}

void AccountManager::loadFromKAccounts()
{
#if HAVE_KACCOUNTS
    qDebug() << "Loading accounts from kaccounts.";

    auto manager = KAccounts::accountsManager();

    for (auto i : KAccounts::accountsManager()->accountList()) {
        auto *a = manager->account(i);
        QString name = a->provider().name();

        if (name == QStringLiteral("mastodon")) {
            auto tokodonAccount = new Account(a->displayName(), a);
            addAccount(tokodonAccount);
        }
    }
#endif
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
