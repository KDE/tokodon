// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QObject>
#include <QSettings>

#include "account.h"
#include "post.h"

class AccountManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasAccounts READ hasAccounts NOTIFY accountAdded NOTIFY accountRemoved)
    Q_PROPERTY(Account *selectedAccount READ selectedAccount WRITE selectAccount NOTIFY accountSelected)
public:
    static AccountManager &instance();

    void loadFromSettings(QSettings &settings);
    void writeToSettings(QSettings &settings);

    bool hasAccounts() const;
    Q_INVOKABLE void addAccount(Account *account);
    Q_INVOKABLE void removeAccount(Account *account);

    void selectAccount(Account *account);
    Account *selectedAccount();

    Q_INVOKABLE Account *createNewAccount(const QString &username, const QString &instanceUri);

Q_SIGNALS:
    void accountAdded(Account *account);
    void accountRemoved(Account *account);
    void accountSelected(Account *account);
    void identityChanged(Account *account);
    void fetchedTimeline(Account *account, QString original_name, QList<std::shared_ptr<Post>> posts);
    void invalidated(Account *account);
    void fetchedInstanceMetadata(Account *account);
    void invalidatedPost(Account *account, Post *post);
    void notification(Account *account, std::shared_ptr<Notification> n);

public Q_SLOTS:
    void childIdentityChanged(Account *account);

private:
    explicit AccountManager(QObject *parent = nullptr);
    virtual ~AccountManager();
    QList<Account *> m_accounts;
    Account *m_selected_account;
};
