// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <KAboutData>
#include <QAbstractListModel>
#include <QSettings>

#include "post.h"

class AbstractAccount;
class QNetworkAccessManager;
class AccountModel;

class AccountManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool hasAccounts READ hasAccounts NOTIFY accountAdded NOTIFY accountRemoved)
    Q_PROPERTY(AbstractAccount *selectedAccount READ selectedAccount WRITE selectAccount NOTIFY accountSelected)
    Q_PROPERTY(QVariant selectedAccountModel READ selectedAccountModel NOTIFY accountSelected)
    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY accountSelected)
    Q_PROPERTY(KAboutData aboutData READ aboutData WRITE setAboutData NOTIFY aboutDataChanged)
public:
    enum CustomRoles {
        AccountRole = Qt::UserRole + 1,
        DescriptionRole,
        InstanceRole,
    };

    static AccountManager &instance();

    void loadFromSettings(QSettings &settings);
    void writeToSettings(QSettings &settings);

    bool hasAccounts() const;
    Q_INVOKABLE void addAccount(AbstractAccount *account);
    Q_INVOKABLE void removeAccount(AbstractAccount *account);

    void selectAccount(AbstractAccount *account, bool explicitUserAction = true);
    AbstractAccount *selectedAccount() const;
    QVariant selectedAccountModel();

    int selectedIndex() const;

    void setAboutData(const KAboutData &aboutData);
    [[nodiscard]] KAboutData aboutData() const;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE AbstractAccount *createNewAccount(const QString &username, const QString &instanceUri, bool ignoreSslErrors = false);

Q_SIGNALS:
    void accountAdded(AbstractAccount *account);
    void accountRemoved(AbstractAccount *account);
    void accountSelected(AbstractAccount *account);
    void identityChanged(AbstractAccount *account);
    void fetchedTimeline(AbstractAccount *account, QString original_name, QList<Post *> posts);
    void invalidated(AbstractAccount *account);
    void fetchedInstanceMetadata(AbstractAccount *account);
    void invalidatedPost(AbstractAccount *account, Post *post);
    void notification(AbstractAccount *account, std::shared_ptr<Notification> n);
    void aboutDataChanged();

public Q_SLOTS:
    void childIdentityChanged(AbstractAccount *account);

private:
    explicit AccountManager(QObject *parent = nullptr);
    virtual ~AccountManager();
    QList<AbstractAccount *> m_accounts;
    AbstractAccount *m_selected_account;
    KAboutData m_aboutData;
    QNetworkAccessManager *m_qnam;
};
