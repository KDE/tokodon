// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>

#pragma once

#include <AbstractKirigamiApplication>
#include <QObject>

#include "account/accountmanager.h"

class TokodonApplication : public AbstractKirigamiApplication
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(AccountManager *accountManager READ accountManager WRITE setAccountManager NOTIFY accountManagerChanged)

public:
    explicit TokodonApplication(QObject *parent = nullptr);
    ~TokodonApplication() override = default;

    [[nodiscard]] AccountManager *accountManager() const;
    void setAccountManager(AccountManager *accountManager);

    [[nodiscard]] QList<KirigamiActionCollection *> actionCollections() const override;

    /**
     * @brief Visually unchecks the main page actions, if needed.
     */
    Q_INVOKABLE void uncheckMainActions();

Q_SIGNALS:
    void accountManagerChanged();
    void configureAccount(AbstractAccount *account);
    void addAccount();
    void openHomeTimeline();
    void openNotifications();
    void openFollowRequests();
    void openLocalTimeline();
    void openGlobalTimeline();
    void openConversations();
    void openFavorites();
    void openBookmarks();
    void openExplore();
    void openFollowing();
    void openSearch();
    void openServerInformation();
    void openLists();
    void openProfile();
    void openAccountSwitcher();
    void searchFor(const QString &query);

private:
    void setupActions() override;
    void setupAccountCollection();
    void switchAccount(AbstractAccount *account);
    void createAccountActions(AbstractAccount *account);
    void updateAccountActions();

    AccountManager *m_accountManager = nullptr;
    KirigamiActionCollection *m_accountCollection = nullptr;
    QHash<AbstractAccount *, QAction *> m_switchUserActions;
    QHash<AbstractAccount *, QAction *> m_configureUserActions;
    QAction *m_dummyAction;
};
