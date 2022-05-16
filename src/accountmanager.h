// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <KAboutData>
#include <QAbstractListModel>
#include <QSettings>

#include "account.h"
#include "post.h"

class AccountManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool hasAccounts READ hasAccounts NOTIFY accountAdded NOTIFY accountRemoved)
    Q_PROPERTY(Account *selectedAccount READ selectedAccount WRITE selectAccount NOTIFY accountSelected)
    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY accountSelected)
    Q_PROPERTY(KAboutData aboutData READ aboutData WRITE setAboutData NOTIFY aboutDataChanged)
public:
    enum CustomRoles {
        AccountRole = Qt::UserRole + 1,
        DescriptionRole,
    };

    static AccountManager &instance();

    void loadFromSettings(QSettings &settings);
    void writeToSettings(QSettings &settings);

    bool hasAccounts() const;
    Q_INVOKABLE void addAccount(Account *account);
    Q_INVOKABLE void removeAccount(Account *account);

    void selectAccount(Account *account);
    Account *selectedAccount() const;

    int selectedIndex() const;

    void setAboutData(const KAboutData &aboutData);
    [[nodiscard]] KAboutData aboutData() const;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE Account *createNewAccount(const QString &username, const QString &instanceUri, bool ignoreSslErrors = false);

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
    void aboutDataChanged();

public Q_SLOTS:
    void childIdentityChanged(Account *account);

private:
    explicit AccountManager(QObject *parent = nullptr);
    virtual ~AccountManager();
    QList<Account *> m_accounts;
    Account *m_selected_account;
    KAboutData m_aboutData;
};
