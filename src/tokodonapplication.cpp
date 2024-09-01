// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "tokodonapplication.h"
#include "navigation.h"

#include <KAuthorized>
#include <KLocalizedString>

using namespace Qt::StringLiterals;

TokodonApplication::TokodonApplication(QObject *parent)
    : AbstractKirigamiApplication(parent)
    , m_accountCollection(new KirigamiActionCollection(this, i18nc("@title:group Title for the group of account related actions", "Account")))
{
    setupActions();
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
        disconnect(m_accountManager, &AccountManager::accountAdded, this, nullptr);
        disconnect(m_accountManager, &AccountManager::accountRemoved, this, nullptr);
        disconnect(m_accountManager, &AccountManager::identityChanged, this, nullptr);
        m_accountCollection->clear();
    }

    m_accountManager = accountManager;
    Q_EMIT accountManagerChanged();

    if (m_accountManager) {
        setupAccountCollection();

        connect(m_accountManager, &AccountManager::accountAdded, this, [this](AbstractAccount *account) {
            createAccountActions(account);
        });

        connect(m_accountManager, &AccountManager::accountRemoved, this, [this](AbstractAccount *account) {
            if (m_switchUserActions[account]) {
                m_accountCollection->removeAction(m_switchUserActions[account]);
            }
        });

        connect(m_accountManager, &AccountManager::identityChanged, this, [this](AbstractAccount *account) {
            if (m_switchUserActions[account]) {
                m_switchUserActions[account]->setText(i18nc("@action:button", "Switch to '%1'", account->identity()->displayNameHtml()));
                m_configureUserActions[account]->setText(i18nc("@action:button", "Configure '%1'", account->identity()->displayNameHtml()));
            }
        });

        auto actionName = u"add_account"_s;
        if (KAuthorized::authorizeAction(actionName)) {
            auto addAccount = m_accountCollection->addAction(actionName, this, &TokodonApplication::addAccount);
            addAccount->setText(i18nc("@action:button", "Add Account"));
            addAccount->setIcon(QIcon::fromTheme(QStringLiteral("list-add-user-symbolic")));
        }

        readSettings();
    }
}

void TokodonApplication::setupActions()
{
    AbstractKirigamiApplication::setupActions();

    auto configureAction = mainCollection()->addAction(u"open_status_composer"_s, this, []() {
        Q_EMIT Navigation::instance().openComposer(QString{});
    });
    configureAction->setText(i18nc("@action:button", "Write a New Post"));
    configureAction->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));

    mainCollection()->addAction(configureAction->objectName(), configureAction);
    mainCollection()->setDefaultShortcut(configureAction, QKeySequence(Qt::CTRL | Qt::Key_N));
}

void TokodonApplication::setupAccountCollection()
{
    Q_ASSERT(m_accountManager);

    for (auto account : m_accountManager->accounts()) {
        createAccountActions(account);
    }
}

void TokodonApplication::switchAccount(AbstractAccount *account)
{
    Q_ASSERT(m_accountManager);

    if (m_accountManager->selectedAccount() != account) {
        m_accountManager->selectAccount(account);
    }
}

void TokodonApplication::createAccountActions(AbstractAccount *account)
{
    auto switchAction = m_accountCollection->addAction(u"switch_account_"_s + account->username() + account->instanceUri(), this, [this, account]() {
        switchAccount(account);
    });
    // We are intentionally using AbstractAccount::username() here because the identity may not be filled out yet
    // Either because the account is not yet loaded, or there was an error while loading it.
    switchAction->setText(i18nc("@action:button", "Switch '%1'", account->username()));
    switchAction->setIcon(QIcon::fromTheme(QStringLiteral("system-switch-user")));
    m_switchUserActions[account] = switchAction;
    m_accountCollection->setShortcutsConfigurable(switchAction, false);

    auto configureAction = m_accountCollection->addAction(u"configure_account_"_s + account->username() + account->instanceUri(), this, [this, account]() {
        Q_EMIT configureAccount(account);
    });
    configureAction->setText(i18nc("@action:button", "Configure '%1'", account->username()));
    configureAction->setIcon(QIcon::fromTheme(QStringLiteral("im-user-symbolic")));
    m_configureUserActions[account] = configureAction;
    m_accountCollection->setShortcutsConfigurable(configureAction, false);
}

QList<KirigamiActionCollection *> TokodonApplication::actionCollections() const
{
    return {
        mainCollection(),
        m_accountCollection,
    };
}

#include "moc_tokodonapplication.cpp"
