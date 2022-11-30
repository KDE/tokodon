// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "accountmanager.h"
#include "account.h"
#include "networkaccessmanagerfactory.h"

AccountManager::AccountManager(QObject *parent)
    : QAbstractListModel(parent)
    , m_selected_account(nullptr)
    , m_qnam(NetworkAccessManagerFactory().create(this))
{
    QSettings settings;
    loadFromSettings(settings);
}

AccountManager::~AccountManager()
{
    // before destroying the world, dump everything to QSettings
    QSettings settings;
    writeToSettings(settings);

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
    switch (role) {
    case Qt::DisplayRole:
        return account->identity().m_display_name;
    case DescriptionRole:
        return account->identity().m_acct;
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
    };
}

AccountManager &AccountManager::instance()
{
    static AccountManager accountManager;
    return accountManager;
}

AbstractAccount *AccountManager::createNewAccount(const QString &username, const QString &instanceUri, bool ignoreSslErrors)
{
    return new Account(username, instanceUri, m_qnam, ignoreSslErrors, this);
}

bool AccountManager::hasAccounts() const
{
    return m_accounts.size() > 0;
}

void AccountManager::addAccount(AbstractAccount *account)
{
    beginInsertRows(QModelIndex(), m_accounts.size(), m_accounts.size());
    m_accounts.append(account);
    endInsertRows();

    Q_EMIT accountAdded(account);
    selectAccount(account);

    connect(account, &Account::identityChanged, this, &AccountManager::childIdentityChanged);
    connect(account, &Account::authenticated, this, [this]() {
        Q_EMIT dataChanged(index(0, 0), index(m_accounts.size() - 1, 0));
    });

    connect(account, &Account::fetchedTimeline, this, [this, account](QString original_name, QList<std::shared_ptr<Post>> posts) {
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
    QSettings settings;
    writeToSettings(settings);
}

void AccountManager::childIdentityChanged(AbstractAccount *account)
{
    Q_EMIT identityChanged(account);
}

void AccountManager::removeAccount(AbstractAccount *account)
{
    const auto index = m_accounts.indexOf(account);
    beginRemoveRows(QModelIndex(), index, index);
    m_accounts.removeOne(account);
    endRemoveRows();

    QSettings settings;
    settings.clear();
    writeToSettings(settings);

    Q_EMIT accountRemoved(account);
}

void AccountManager::selectAccount(AbstractAccount *account)
{
    if (!m_accounts.contains(account)) {
        qDebug() << "WTF: attempt to select unmanaged account" << account;
        return;
    }

    m_selected_account = account;

    Q_EMIT accountSelected(account);
}

AbstractAccount *AccountManager::selectedAccount() const
{
    return m_selected_account;
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

void AccountManager::writeToSettings(QSettings &settings)
{
    settings.beginGroup("accounts");

    for (auto a : std::as_const(m_accounts)) {
        a->writeToSettings(settings);
    }

    settings.endGroup();
}

void AccountManager::loadFromSettings(QSettings &settings)
{
    qDebug() << "Loading any accounts from settings.";

    settings.beginGroup("accounts");

    const auto childGroups = settings.childGroups();
    for (const auto &child : childGroups) {
        settings.beginGroup(child);

        auto account = new Account(settings, m_qnam);
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
