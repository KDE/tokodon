// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <KAboutData>
#include <QAbstractListModel>
#include <QJSEngine>
#include <QSettings>
#include <QtQml/qqmlregistration.h>

#include "timeline/post.h"

class AbstractAccount;
class QNetworkAccessManager;

class AccountManager : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    /// Whether or not the account manager is completely ready
    /// This doesn't mean it has accounts, simply that it's done reading configs and the keychain
    Q_PROPERTY(bool isReady READ isReady NOTIFY accountsReady)

    /// If there any valid accounts loaded
    Q_PROPERTY(bool hasAccounts READ hasAccounts NOTIFY accountsChanged)

    // If there are any accounts in the config
    Q_PROPERTY(bool hasAnyAccounts READ hasAnyAccounts NOTIFY accountsChanged)

    /// The currently selected account
    Q_PROPERTY(AbstractAccount *selectedAccount READ selectedAccount WRITE selectAccount NOTIFY accountSelected)

    /// The currently seelcted account's id
    Q_PROPERTY(QString selectedAccountId READ selectedAccountId NOTIFY accountSelected)

    /// The index of the seelcted account in the account list
    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY accountSelected)

    /// The about data of the application
    Q_PROPERTY(KAboutData aboutData READ aboutData WRITE setAboutData NOTIFY aboutDataChanged)

    /// Whether or not we're running a Flatpak build
    Q_PROPERTY(bool isFlatpak READ isFlatpak CONSTANT)

public:
    static AccountManager *create(QQmlEngine *, QJSEngine *)
    {
        auto inst = &instance();
        QJSEngine::setObjectOwnership(inst, QJSEngine::ObjectOwnership::CppOwnership);
        return inst;
    }

    enum CustomRoles {
        AccountRole = Qt::UserRole + 1,
        DisplayNameRole,
        DescriptionRole,
        InstanceRole,
    };

    static AccountManager &instance();

    void loadFromSettings();
    void migrateSettings();

    bool isReady() const;
    bool hasAccounts() const;
    bool hasAnyAccounts() const;
    Q_INVOKABLE void addAccount(AbstractAccount *account);
    Q_INVOKABLE void removeAccount(AbstractAccount *account);
    void reloadAccounts();

    void selectAccount(AbstractAccount *account, bool explicitUserAction = true);
    AbstractAccount *selectedAccount() const;
    QString selectedAccountId() const;

    int selectedIndex() const;

    void setAboutData(const KAboutData &aboutData);
    [[nodiscard]] KAboutData aboutData() const;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE AbstractAccount *createNewAccount(const QString &instanceUri, bool ignoreSslErrors = false, bool admin = true);

    bool isFlatpak() const;

    /// Returns the preferred settings group name for an account name and an instance uri.
    /// It's preferred to use AbstractAccount::settingsGroupName as it fills in the relevant information.
    static QString settingsGroupName(const QString &name, const QString &instanceUri);

    /// Returns the preferred key name for the client secret given a settings group name.
    /// It's preferred to use AbstractAccount::clientSecretKey as it fills in the relevant information.
    static QString clientSecretKey(const QString &name);

    /// Returns the preferred key name for the access token.
    /// It's preferred to use AbstractAccount::accessTokenKey as it fills in the relevant information.
    static QString accessTokenKey(const QString &name);

Q_SIGNALS:

    void accountAdded(AbstractAccount *account);

    void accountRemoved(AbstractAccount *account);

    void accountsChanged();

    void accountsReady();

    void accountsReloaded();

    void accountSelected(AbstractAccount *account);

    void identityChanged(AbstractAccount *account);

    void fetchedTimeline(AbstractAccount *account, QString original_name, QList<Post *> posts);

    void invalidated(AbstractAccount *account);

    void fetchedInstanceMetadata(AbstractAccount *account);

    void invalidatedPost(AbstractAccount *account, Post *post);

    void notification(AbstractAccount *account, std::shared_ptr<Notification> n);

    void aboutDataChanged();

    void webapLink(QString id);

public Q_SLOTS:

    void childIdentityChanged(AbstractAccount *account);

private:
    explicit AccountManager(QObject *parent = nullptr);

    ~AccountManager() override;

    QList<AbstractAccount *> m_accounts;
    AbstractAccount *m_selected_account = nullptr;
    KAboutData m_aboutData;
    QNetworkAccessManager *m_qnam;

    enum class AccountStatus { NotLoaded, Loaded, InvalidCredentials };

    QList<AccountStatus> m_accountStatus;

    bool m_ready = false;
    bool m_hasAnyAccounts = false;

    void checkIfLoadingFinished();
};
