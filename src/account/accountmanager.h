// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "timeline/notification.h"
#include "timeline/post.h"

#include <KAboutData>

#include <QAbstractListModel>
#include <QJSEngine>

class AbstractAccount;
class QNetworkAccessManager;

/**
 * @brief Handles managing accounts in Tokodon, and tracks state such as which one is currently selected.
 */
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

    /**
     * @brief Custom roles for the AccountManager model.
     */
    enum CustomRoles {
        AccountRole = Qt::UserRole + 1, /**< Account object. */
        DisplayNameRole, /**< Display name of the account. Uses the display name if set, otherwise falls back to the username. */
        DescriptionRole, /**< Username of the account. */
        InstanceRole, /**< Instance name of the account. */
    };

    static AccountManager &instance();

    /**
     * @brief Load accounts from disk
     */
    void loadFromSettings();

    /**
     * @brief Migrates old Tokodon settings to newer formats.
     */
    void migrateSettings();

    /**
     * @brief Enables or disables test mode. Used internally for tokodon-offline.
     * @param enabled Whether test mode should be enabled.
     */
    void setTestMode(bool enabled);

    /**
     * @return If testing mode is enabled.
     */
    bool testMode() const;

    /**
     * @return Whether or not the account manager is completely ready.
     * @note This doesn't mean it has accounts, simply that it's done reading configs and the keychain.
     */
    bool isReady() const;

    /**
     * @return If there any valid accounts loaded.
     */
    bool hasAccounts() const;

    /**
     * @return If there are any accounts in the config.
     */
    bool hasAnyAccounts() const;

    /**
     * @brief Adds a new account.
     * @param account The account to manage.
     * @param skipAuthenticationCheck Whether the account manager should internally check if the account is valid.
     */
    Q_INVOKABLE void addAccount(AbstractAccount *account, bool skipAuthenticationCheck);

    /**
     * @brief Removes an existing account.
     * @param account The account to remove.
     */
    Q_INVOKABLE void removeAccount(AbstractAccount *account);

    /**
     * @brief Re-validates every account's credentials.
     */
    void reloadAccounts();

    void queueNotifications();

    /**
     * @return If the currently selected account has issues with authentication.
     */
    Q_INVOKABLE bool selectedAccountHasIssue() const;

    /**
     * @return If the selected account has a login issue, returns a localized string explaining why.
     */
    Q_INVOKABLE QString selectedAccountLoginIssue() const;

    /**
     * @brief Switches to an existing account.
     * @param account The account to switch to.
     * @param explicitUserAction If true, considers this an explicit user action and the new selected account will be written to disk.
     */
    void selectAccount(AbstractAccount *account, bool explicitUserAction = true);

    /**
     * @return The currently selected account.
     */
    AbstractAccount *selectedAccount() const;

    /**
     * @return The currently selected account's id.
     */
    QString selectedAccountId() const;

    /**
     * @return The index of the selected account in the account list.
     */
    int selectedIndex() const;

    /**
     * @brief Sets the application about data.
     * @param aboutData The new about data.
     */
    void setAboutData(const KAboutData &aboutData);

    /**
     * @return The application's about data.
     */
    [[nodiscard]] KAboutData aboutData() const;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Creates a new account, and adds it to the manager.
     * @param instanceUri The URI of the instance.
     * @param ignoreSslErrors Whether or ignore SSL errors from this URI.
     * @param admin Request admin scopes.
     * @return The newly created account.
     */
    Q_INVOKABLE AbstractAccount *createNewAccount(const QString &instanceUri, bool ignoreSslErrors = false, bool admin = true);

    /**
     * @return Whether or not Tokodon is built as a Flatpak.
     */
    bool isFlatpak() const;

    /**
     * @note It's preferred to use AbstractAccount::settingsGroupName as it fills in the relevant information.
     * @return The preferred settings group name for an account name and an instance uri.
     */
    static QString settingsGroupName(const QString &name, const QString &instanceUri);

    /**
     * @note It's preferred to use AbstractAccount::clientSecretKey as it fills in the relevant information.
     * @param name The settings group name, from AbstractAccount::settingsGroupName().
     * @return The preferred key name for the client secret given a settings group name.
     */
    static QString clientSecretKey(const QString &name);

    /**
     * @note It's preferred to use AbstractAccount::accessTokenKey as it fills in the relevant information.
     * @param name The settings group name, from AbstractAccount::settingsGroupName().
     * @return The preferred key name for the access token.
     */
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
