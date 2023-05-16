// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFlags>
#include <QJsonArray>
#include <QObject>
#include <QSettings>
#include <QUrl>
#include <QJsonArray>

class AbstractAccount;
class Identity;

class AdminAccountInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool emailStatus READ emailStatus NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(bool suspended READ suspended WRITE setSuspended NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(bool silenced READ silenced WRITE setSilence NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(bool sensitized READ sensitized WRITE setSensitized NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(bool disabled READ disabled WRITE setDisabled NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(bool approved READ approved WRITE setApproved NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(bool isLocal READ isLocal NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(QString loginStatus READ loginStatus NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(QString inviteRequest READ inviteRequest NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(QString ip READ ip NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(QString email READ email NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(QString emailProvider READ emailProvider NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(QString locale READ locale NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(QString joined READ joined NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(QString lastActive READ lastActive NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(QString role READ role NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(QJsonArray ips READ ips NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(Identity *invitedByIdentity READ invitedByIdentity NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(Identity *userLevelIdentity READ userLevelIdentity NOTIFY adminAccountInfoUpdated)
    Q_PROPERTY(int position READ position NOTIFY adminAccountInfoUpdated)

public:
    enum Permission {
        Administrator = 0x1,
        Devops = 0x2,
        ViewAuditLog = 0x4,
        ViewDashboard = 0x8,
        ManageReports = 0x10,
        ManageFederation = 0x20,
        ManageSettings = 0x40,
        ManageBlocks = 0x80,
        ManageTaxonomies = 0x100,
        ManageAppeals = 0x200,
        ManageUsers = 0x400,
        ManageInvites = 0x800,
        ManageRules = 0x1000,
        ManageAnnouncements = 0x2000,
        ManageCustomEmojis = 0x4000,
        ManageWebhooks = 0x8000,
        InviteUsers = 0x10000,
        ManageRoles = 0x20000,
        ManageUserAccess = 0x40000,
        DeleteUserData = 0x80000,
        ModerationToolAccess = Administrator | Devops | ViewAuditLog | ViewDashboard | ManageReports | ManageFederation | ManageSettings | ManageBlocks
            | ManageTaxonomies | ManageAppeals | ManageUsers | ManageInvites | ManageRules | ManageAnnouncements | ManageCustomEmojis | ManageWebhooks
            | InviteUsers | ManageRoles | ManageUserAccess | DeleteUserData
    };

    Q_DECLARE_FLAGS(Permissions, Permission)
    Q_ENUM(Permission)
    QString ip() const;
    QString email() const;
    QString emailProvider() const;
    QString locale() const;
    QString joined() const;
    QString lastActive() const;
    QString role() const;
    QString loginStatus() const;
    QString inviteRequest() const;
    bool emailStatus() const;
    bool suspended() const;
    void setSuspended(bool suspended);
    bool silenced() const;
    void setSilence(bool silenced);
    bool sensitized() const;
    void setSensitized(bool sensitized);
    bool disabled() const;
    void setDisabled(bool disabled);
    bool approved() const;
    void setApproved(bool approved);
    bool isLocal() const;
    int position() const;

    QJsonArray fields() const;
    QJsonArray ips() const;

    void fromSourceData(const QJsonObject &jdoc);
    void reparentAdminAccountInfo(AbstractAccount *parent);

    Identity *invitedByIdentity() const;
    Identity *userLevelIdentity() const;

Q_SIGNALS:
    void adminAccountInfoUpdated();

private:
    QString m_ip;
    QString m_email;
    QString m_emailProvider;
    QString m_locale;
    QString m_joined;
    QString m_lastActive;
    QString m_role;
    QString m_inviteRequest;
    bool m_emailStatus;
    bool m_suspended;
    bool m_silenced;
    bool m_sensitized;
    bool m_disabled;
    bool m_approved;
    int m_position;
    QJsonArray m_ips;
    std::shared_ptr<Identity> m_invitedByIdentity;
    std::shared_ptr<Identity> m_userLevelIdentity;
    AbstractAccount *m_parent = nullptr;
};
