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

/// Handles managing accounts in Tokodon, and tracks state such as which one is currently selected.
class AccountManager : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool isReady READ isReady NOTIFY accountsReady)
    Q_PROPERTY(bool hasAccounts READ hasAccounts NOTIFY accountsChanged)
    Q_PROPERTY(bool hasAnyAccounts READ hasAnyAccounts NOTIFY accountsChanged)
    Q_PROPERTY(AbstractAccount *selectedAccount READ selectedAccount WRITE selectAccount NOTIFY accountSelected)
    Q_PROPERTY(QString selectedAccountId READ selectedAccountId NOTIFY accountSelected)
    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY accountSelected)
    Q_PROPERTY(KAboutData aboutData READ aboutData WRITE setAboutData NOTIFY aboutDataChanged)
    Q_PROPERTY(bool isFlatpak READ isFlatpak CONSTANT)
    Q_PROPERTY(bool selectedAccountHasIssue READ selectedAccountHasIssue NOTIFY accountSelected)
    Q_PROPERTY(bool testMode READ testMode CONSTANT)

public:
    static AccountManager *create(QQmlEngine *, QJSEngine *)
    {
        auto inst = &instance();
        QJSEngine::setObjectOwnership(inst, QJSEngine::ObjectOwnership::CppOwnership);
        return inst;
    }

    /// Custom roles for the AccountManager model
    enum CustomRoles {
        AccountRole = Qt::UserRole + 1, ///< Account object
        DisplayNameRole, ///< Display name of the account. Uses the display name if set, otherwise falls back to the username
        DescriptionRole, ///< Username of the account
        InstanceRole, ///< Instance name of the account
    };

    static AccountManager &instance();

    /// Load accounts from disk
    void loadFromSettings();

    /// Migrates old Tokodon settings to newer formats
    void migrateSettings();

    /// Enables or disables test mode. Used internally for tokodon-offline
    /// \param enabled Whether test mode should be enabled
    void setTestMode(bool enabled);

    /// Returns if testing mode is enabled
    bool testMode() const;

    /// Whether or not the account manager is completely ready
    /// This doesn't mean it has accounts, simply that it's done reading configs and the keychain
    bool isReady() const;

    /// If there any valid accounts loaded
    bool hasAccounts() const;

    /// If there are any accounts in the config
    bool hasAnyAccounts() const;

    /// Adds a new account
    /// \param account The account to manage
    /// \param skipAuthenticationCheck Whether the account manager should internally check if the account is valid
    Q_INVOKABLE void addAccount(AbstractAccount *account, bool skipAuthenticationCheck);

    /// Removes an existing account
    /// \param account The account to remove
    Q_INVOKABLE void removeAccount(AbstractAccount *account);

    /// Re-validates every account's credentials
    void reloadAccounts();
    void queueNotifications();

    /// Returns if the currently selected account has issues with authentication
    Q_INVOKABLE bool selectedAccountHasIssue() const;

    /// If the selected account has a login issue, returns a localized string explaining why
    Q_INVOKABLE QString selectedAccountLoginIssue() const;

    /// Switches to an existing account
    /// \param explicitUserAction If true, considers this an explicit user action and the new selected account will be written to disk
    void selectAccount(AbstractAccount *account, bool explicitUserAction = true);

    /// The currently selected account
    AbstractAccount *selectedAccount() const;

    /// The currently selected account's id
    QString selectedAccountId() const;

    /// The index of the selected account in the account list
    int selectedIndex() const;

    /// Sets the application about data
    /// \param aboutData The new about data
    void setAboutData(const KAboutData &aboutData);

    /// Returns the application's about data
    [[nodiscard]] KAboutData aboutData() const;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    /// Creates a new account, and adds it to the manager
    /// \param instanceUri The URI of the instance
    /// \param ignoreSslErrors Whether or ignore SSL errors from this URI
    /// \param admin Request admin scopes
    Q_INVOKABLE AbstractAccount *createNewAccount(const QString &instanceUri, bool ignoreSslErrors = false, bool admin = true);

    /// Returns whether or not Tokodon is built as a Flatpak
    bool isFlatpak() const;

    /// Returns the preferred settings group name for an account name and an instance uri.
    /// It's preferred to use AbstractAccount::settingsGroupName as it fills in the relevant information.
    static QString settingsGroupName(const QString &name, const QString &instanceUri);

    /// Returns the preferred key name for the client secret given a settings group name.
    /// It's preferred to use AbstractAccount::clientSecretKey as it fills in the relevant information.
    /// \param name The settings group name, from AbstractAccount::settingsGroupName()
    static QString clientSecretKey(const QString &name);

    /// Returns the preferred key name for the access token.
    /// It's preferred to use AbstractAccount::accessTokenKey as it fills in the relevant information.
    /// \param name The settings group name, from AbstractAccount::settingsGroupName()
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

    void finishedNotificationQueue();

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
    QList<QString> m_accountStatusStrings;

    bool m_ready = false;
    bool m_hasAnyAccounts = false;
    bool m_testMode = false;

    void checkIfLoadingFinished();
};
