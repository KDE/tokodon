// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "tokodonapplication.h"
#include "navigation.h"

#include <KAuthorized>
#include <KLocalizedString>
#include <QActionGroup>

#include "account.h"

using namespace Qt::StringLiterals;

TokodonApplication::TokodonApplication(QObject *parent)
    : QObject(parent)
//, m_accountCollection(new KirigamiActionCollection(this, i18nc("@title:group Title for the group of account related actions", "Account")))
{
}

AccountManager *TokodonApplication::accountManager() const
{
    return m_accountManager;
}

void TokodonApplication::setAccountManager(AccountManager *accountManager)
{
    if (m_accountManager == accountManager) {
        return;
    }

    if (m_accountManager) {
        disconnect(m_accountManager, &AccountManager::accountsChanged, this, nullptr);
        disconnect(m_accountManager, &AccountManager::accountsReady, this, nullptr);
        disconnect(m_accountManager, &AccountManager::accountAdded, this, nullptr);
        disconnect(m_accountManager, &AccountManager::accountRemoved, this, nullptr);
        disconnect(m_accountManager, &AccountManager::identityChanged, this, nullptr);
    }

    m_accountManager = accountManager;
    Q_EMIT accountManagerChanged();

    /* TODO: kauthorized
    auto actionName = u"add_account"_s;
    if (KAuthorized::authorizeAction(actionName)) {
        auto addAccount = m_accountCollection->addAction(actionName, this, &TokodonApplication::addAccount);
        addAccount->setText(i18nc("@action:button", "Add Account"));
        addAccount->setIcon(QIcon::fromTheme(QStringLiteral("list-add-user-symbolic")));
    }
    */
}

void TokodonApplication::switchAccount(AbstractAccount *account)
{
    Q_ASSERT(m_accountManager);

    if (m_accountManager->selectedAccount() != account) {
        m_accountManager->selectAccount(account);
    }
}

#include "moc_tokodonapplication.cpp"
