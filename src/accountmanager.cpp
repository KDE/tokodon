// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "accountmanager.h"

AccountManager::AccountManager(QObject *parent)
    : QObject(parent)
    , m_selected_account(nullptr)
{
    QSettings settings;
    loadFromSettings(settings);
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
    m_accounts.append(account);

    Q_EMIT accountAdded(account);
    selectAccount(account);

    QObject::connect(account, &Account::identityChanged, this, &AccountManager::childIdentityChanged);
    QObject::connect(account, &Account::fetchedTimeline, [=] (QString original_name, QList<std::shared_ptr<Post>> posts) {
        Q_EMIT fetchedTimeline(account, original_name, posts);
    });
    QObject::connect(account, &Account::invalidated, [=] () {
        Q_EMIT invalidated(account);
    });
    QObject::connect(account, &Account::fetchedInstanceMetadata, [=] () {
        Q_EMIT fetchedInstanceMetadata(account);
    });
    QObject::connect(account, &Account::invalidatedPost, [=] (Post *p) {
        Q_EMIT invalidatedPost(account, p);
    });
    QObject::connect(account, &Account::notification, [=] (std::shared_ptr<Notification> n) {
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
    if (! m_accounts.contains(account))
    {
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

    for (auto child : settings.childGroups())
    {
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

KAboutData AccountManager::aboutData() const
{
    return m_aboutData;
}

void AccountManager::setAboutData(const KAboutData &aboutData)
{
    m_aboutData = aboutData;
    Q_EMIT aboutDataChanged();
}
