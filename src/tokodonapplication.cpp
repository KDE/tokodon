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
        disconnect(m_accountManager, &AccountManager::accountsChanged, this, nullptr);
        disconnect(m_accountManager, &AccountManager::accountsReady, this, nullptr);
        disconnect(m_accountManager, &AccountManager::accountAdded, this, nullptr);
        disconnect(m_accountManager, &AccountManager::accountRemoved, this, nullptr);
        disconnect(m_accountManager, &AccountManager::identityChanged, this, nullptr);
        m_accountCollection->clear();
    }

    m_accountManager = accountManager;
    Q_EMIT accountManagerChanged();

    if (m_accountManager) {
        setupAccountCollection();
        updateAccountActions();

        connect(m_accountManager, &AccountManager::accountSelected, this, &TokodonApplication::updateAccountActions);
        connect(m_accountManager, &AccountManager::accountsReady, this, &TokodonApplication::updateAccountActions);

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

    auto pagesGroup = new QActionGroup(this);
    pagesGroup->setExclusive(true);

    auto configureAction = mainCollection()->addAction(u"open_status_composer"_s, this, [] {
        Q_EMIT Navigation::instance().openComposer(QString{});
    });
    configureAction->setCheckable(false);
    configureAction->setText(i18nc("@action:button", "Write a New Post"));
    configureAction->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));
    mainCollection()->setDefaultShortcut(configureAction, QKeySequence(Qt::CTRL | Qt::Key_N));

    auto homeTimelineAction = mainCollection()->addAction(u"home_timeline"_s, this, &TokodonApplication::openHomeTimeline);
    homeTimelineAction->setCheckable(true);
    homeTimelineAction->setActionGroup(pagesGroup);
    homeTimelineAction->setText(i18nc("@action:button Home Timeline", "Open Home Timeline"));
    homeTimelineAction->setIcon(QIcon::fromTheme(QStringLiteral("go-home-large")));

    auto notificationsAction = mainCollection()->addAction(u"notifications"_s, this, &TokodonApplication::openNotifications);
    notificationsAction->setCheckable(true);
    notificationsAction->setActionGroup(pagesGroup);
    notificationsAction->setText(i18nc("@action:button Account Notifications", "Open Notifications"));
    notificationsAction->setIcon(QIcon::fromTheme(QStringLiteral("notifications")));

    auto followRequestsAction = mainCollection()->addAction(u"follow_requests"_s, this, &TokodonApplication::openFollowRequests);
    followRequestsAction->setCheckable(true);
    followRequestsAction->setActionGroup(pagesGroup);
    followRequestsAction->setText(i18nc("@action:button Follows that require explicit allow/deny", "Open Follow Requests"));
    followRequestsAction->setIcon(QIcon::fromTheme(QStringLiteral("list-add-user")));

    auto localTimelineAction = mainCollection()->addAction(u"local_timeline"_s, this, &TokodonApplication::openLocalTimeline);
    localTimelineAction->setCheckable(true);
    localTimelineAction->setActionGroup(pagesGroup);
    localTimelineAction->setText(i18nc("@action:button Local timeline of posts from the account's own server", "Open Local Timeline"));
    localTimelineAction->setIcon(QIcon::fromTheme(QStringLiteral("system-users")));

    auto globalTimelineAction = mainCollection()->addAction(u"global_timeline"_s, this, &TokodonApplication::openGlobalTimeline);
    globalTimelineAction->setCheckable(true);
    globalTimelineAction->setActionGroup(pagesGroup);
    globalTimelineAction->setText(i18nc("@action:button Global timeline of posts from the entire Fediverse network", "Open Global Timeline"));
    globalTimelineAction->setIcon(QIcon::fromTheme(QStringLiteral("kstars_xplanet")));

    auto conversationsAction = mainCollection()->addAction(u"conversations"_s, this, &TokodonApplication::openConversations);
    conversationsAction->setCheckable(true);
    conversationsAction->setActionGroup(pagesGroup);
    conversationsAction->setText(i18nc("@action:button Direct one-on-one messages between users", "Open Conversations"));
    conversationsAction->setIcon(QIcon::fromTheme(QStringLiteral("view-conversation-balloon-symbolic")));

    auto favoritesAction = mainCollection()->addAction(u"favorites"_s, this, &TokodonApplication::openFavorites);
    favoritesAction->setCheckable(true);
    favoritesAction->setActionGroup(pagesGroup);
    favoritesAction->setText(i18nc("@action:button This account's favorited posts", "Open Favorites"));
    favoritesAction->setIcon(QIcon::fromTheme(QStringLiteral("favorite")));

    auto bookmarksAction = mainCollection()->addAction(u"bookmarks"_s, this, &TokodonApplication::openBookmarks);
    bookmarksAction->setCheckable(true);
    bookmarksAction->setActionGroup(pagesGroup);
    bookmarksAction->setText(i18nc("@action:button This account's bookmarked posts", "Open Bookmarks"));
    bookmarksAction->setIcon(QIcon::fromTheme(QStringLiteral("bookmarks")));

    auto exploreAction = mainCollection()->addAction(u"explore"_s, this, &TokodonApplication::openExplore);
    exploreAction->setCheckable(true);
    exploreAction->setActionGroup(pagesGroup);
    exploreAction->setText(i18nc("@action:button Explore this server's trending posts, news, and more", "Open Explore"));
    exploreAction->setIcon(QIcon::fromTheme(QStringLiteral("kstars_planets")));

    auto followingAction = mainCollection()->addAction(u"following"_s, this, &TokodonApplication::openFollowing);
    followingAction->setCheckable(true);
    followingAction->setActionGroup(pagesGroup);
    followingAction->setText(i18nc("@action:button A list of this account's followed accounts", "Open Following"));
    followingAction->setIcon(QIcon::fromTheme(QStringLiteral("user-group-properties-symbolic")));

    auto searchAction = mainCollection()->addAction(u"search"_s, this, &TokodonApplication::openSearch);
    searchAction->setCheckable(true);
    searchAction->setActionGroup(pagesGroup);
    searchAction->setText(i18nc("@action:button Search for users, posts and tags", "Open Search"));
    searchAction->setIcon(QIcon::fromTheme(QStringLiteral("search")));

    auto serverInformationAction = mainCollection()->addAction(u"server_information"_s, this, &TokodonApplication::openServerInformation);
    serverInformationAction->setCheckable(true);
    serverInformationAction->setActionGroup(pagesGroup);
    serverInformationAction->setText(i18nc("@action:button", "Open Server Information"));
    serverInformationAction->setIcon(QIcon::fromTheme(QStringLiteral("note")));

    auto listsAction = mainCollection()->addAction(u"lists"_s, this, &TokodonApplication::openLists);
    listsAction->setCheckable(true);
    listsAction->setActionGroup(pagesGroup);
    listsAction->setText(i18nc("@action:button This account's lists, or timelines consisting of a groups of accounts", "Open Lists"));
    listsAction->setIcon(QIcon::fromTheme(QStringLiteral("view-list-text")));

    auto profileAction = mainCollection()->addAction(u"profile"_s, this, &TokodonApplication::openProfile);
    profileAction->setCheckable(true);
    profileAction->setActionGroup(pagesGroup);
    profileAction->setText(i18nc("@action:button This account's profile", "Open Profile"));
    profileAction->setIcon(QIcon::fromTheme(QStringLiteral("user")));

    m_dummyAction = new QAction();
    m_dummyAction->setCheckable(true);
    m_dummyAction->setActionGroup(pagesGroup);

    updateAccountActions();
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
    auto switchAction = m_accountCollection->addAction(u"switch_account_"_s + account->username() + account->instanceUri(), this, [this, account] {
        switchAccount(account);
    });
    // We are intentionally using AbstractAccount::username() here because the identity may not be filled out yet
    // Either because the account is not yet loaded, or there was an error while loading it.
    switchAction->setText(i18nc("@action:button", "Switch '%1'", account->username()));
    switchAction->setIcon(QIcon::fromTheme(QStringLiteral("system-switch-user")));
    m_switchUserActions[account] = switchAction;
    m_accountCollection->setShortcutsConfigurable(switchAction, false);

    auto configureAction = m_accountCollection->addAction(u"configure_account_"_s + account->username() + account->instanceUri(), this, [this, account] {
        Q_EMIT configureAccount(account);
    });
    configureAction->setText(i18nc("@action:button", "Configure '%1'", account->username()));
    configureAction->setIcon(QIcon::fromTheme(QStringLiteral("im-user-symbolic")));
    m_configureUserActions[account] = configureAction;
    m_accountCollection->setShortcutsConfigurable(configureAction, false);
}

void TokodonApplication::updateAccountActions()
{
    const bool accountAvailable = AccountManager::instance().selectedAccount() && !AccountManager::instance().selectedAccountHasIssue();

    mainCollection()->action(u"open_status_composer"_s)->setEnabled(accountAvailable);

    // this action is a special case, where hiding it when disabled makes sense as it may have an empty name
    mainCollection()
        ->action(u"server_information"_s)
        ->setEnabled(AccountManager::instance().selectedAccount() && !AccountManager::instance().selectedAccountHasIssue());
    mainCollection()->action(u"home_timeline"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"notifications"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"follow_requests"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"local_timeline"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"global_timeline"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"following"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"profile"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"lists"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"search"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"explore"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"bookmarks"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"favorites"_s)->setEnabled(accountAvailable);
    mainCollection()->action(u"conversations"_s)->setEnabled(accountAvailable);
}

QList<KirigamiActionCollection *> TokodonApplication::actionCollections() const
{
    return {
        mainCollection(),
        m_accountCollection,
    };
}

void TokodonApplication::uncheckMainActions()
{
    m_dummyAction->trigger();
}

#include "moc_tokodonapplication.cpp"
