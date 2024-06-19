// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>

#pragma once

#include <AbstractKirigamiApplication>
#include <QObject>
#include <QQmlEngine>
#include <QSortFilterProxyModel>

#include "account/accountmanager.h"

class TokodonApplication : public AbstractKirigamiApplication
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(AccountManager *accountManager READ accountManager WRITE setAccountManager NOTIFY accountManagerChanged)

public:
    TokodonApplication(QObject *parent = nullptr);
    ~TokodonApplication() override = default;

    AccountManager *accountManager() const;
    void setAccountManager(AccountManager *accountManager);

    QList<KirigamiActionCollection *> actionCollections() const override;

Q_SIGNALS:
    void openConfigurations();
    void accountManagerChanged();
    void configureAccount(AbstractAccount *account);
    void addAccount();

private:
    void setupActions() override;
    void setupAccountCollection();
    void switchAccount(AbstractAccount *account);
    void createAccountActions(AbstractAccount *account);

    AccountManager *m_accountManager = nullptr;
    KirigamiActionCollection *m_accountCollection = nullptr;
    QHash<AbstractAccount *, QAction *> m_switchUserActions;
    QHash<AbstractAccount *, QAction *> m_configureUserActions;
};