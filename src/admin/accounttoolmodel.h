// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include "account/abstractaccount.h"

class AdminAccountInfo;

class AccountsToolModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    /// This property holds the "Location" value of account tool.
    Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY locationChanged)
    /// This property holds the "moderation Status" value of account tool.
    Q_PROPERTY(QString moderationStatus READ moderationStatus WRITE setModerationStatus NOTIFY moderationStatusChanged)
    /// This property holds the "Role" value of account tool.
    Q_PROPERTY(QString role READ role WRITE setRole NOTIFY roleChanged)
    /// This property holds the "username" value of account tool.
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    /// This property holds the "displayName" value of account tool.
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    /// This property holds the "email" value of account tool.
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    /// This property holds the "ip" value of account tool.
    Q_PROPERTY(QString ip READ ip WRITE setIp NOTIFY ipChanged)
    /// This property holds the position value of the current account which is logged in
    Q_PROPERTY(int selectedAccountPosition READ selectedAccountPosition CONSTANT)

public:
    enum CustomRoles {
        IdentityRole = Qt::UserRole + 1,
    };

    enum AdminAccountAction {
        ApproveAccount,
        RejectAccount,
        ActionAgainstAccount,
        EnableDisabledAccount,
        UnsilenceAccount,
        UnsuspendAccount,
        UnmarkSensitiveAccount,
    };

    explicit AccountsToolModel(QObject *parent = nullptr);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] bool loading() const;
    void setLoading(bool loading);

    [[nodiscard]] QUrlQuery buildQuery() const;

    // location
    [[nodiscard]] QString location() const;
    void setLocation(const QString &location);

    // moderation status
    [[nodiscard]] QString moderationStatus() const;
    void setModerationStatus(const QString &moderationStatus);

    // role
    [[nodiscard]] QString role() const;
    void setRole(const QString &role);

    // UserName
    [[nodiscard]] QString username() const;
    void setUsername(const QString &username);

    // displayName
    [[nodiscard]] QString displayName() const;
    void setDisplayName(const QString &displayName);

    // email
    [[nodiscard]] QString email() const;
    void setEmail(const QString &email);

    // ip
    [[nodiscard]] QString ip() const;
    void setIp(const QString &role);

    [[nodiscard]] int selectedAccountPosition() const;

    // clearing and reloading the model
    void clear();
    // delete account data
    Q_INVOKABLE void deleteAccountData(int row);

    Q_INVOKABLE void approveAccount(int row);
    Q_INVOKABLE void rejectAccount(int row);
    Q_INVOKABLE void enableAccount(int row);
    Q_INVOKABLE void unsilenceAccount(int row);
    Q_INVOKABLE void unsuspendAccount(int row);
    Q_INVOKABLE void unsensitiveAccount(int row);
    Q_INVOKABLE void actionAgainstAccount(int row, const QString &type, const bool &emailWarning, const QString &note);

Q_SIGNALS:
    void loadingChanged();
    void locationChanged();
    void moderationStatusChanged();
    void roleChanged();
    void usernameChanged();
    void displayNameChanged();
    void emailChanged();
    void ipChanged();

protected:
    void fetchSelectedAccountPosition();
    void fetchMore(const QModelIndex &parent) override;
    [[nodiscard]] bool canFetchMore(const QModelIndex &parent) const override;
    void executeAdminAction(int row, AdminAccountAction accountAction, const QJsonObject &extraArguments = {});

private:
    void fillTimeline();

    QList<std::shared_ptr<AdminAccountInfo>> m_accounts;
    bool m_loading = false;
    bool m_pagination = true;

    QString m_username;
    QString m_displayName;
    QString m_email;
    QString m_ip;
    QString m_location;
    QString m_moderationStatus;
    QString m_role;
    std::optional<QUrl> m_next;
    int m_selectedAccountPosition = 0;
};
