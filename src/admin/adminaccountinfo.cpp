// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "adminaccountinfo.h"

#include <KLocalizedString>

#include "abstractaccount.h"
#include "accountmanager.h"

using namespace Qt::Literals::StringLiterals;

QString AdminAccountInfo::role() const
{
    return m_role;
}

QString AdminAccountInfo::loginStatus() const
{
    if (m_suspended) {
        return i18nc("login status", "Suspended");
    } else if (m_silenced) {
        return i18nc("login status", "Limited");
    } else if (m_sensitized) {
        return i18nc("login status", "Sensitized");
    } else if (m_disabled) {
        return i18nc("login status", "Frozen");
    } else if (!m_emailStatus && m_ip != QStringLiteral("")) {
        return i18nc("login status", "Email Not confirmed");
    } else if (!m_approved && m_ip != QStringLiteral("")) {
        return i18nc("login status", "Not Approved");
    } else {
        return i18nc("login status", "No Limits Imposed");
    }
}

QString AdminAccountInfo::inviteRequest() const
{
    return m_inviteRequest;
}

bool AdminAccountInfo::emailStatus() const
{
    return m_emailStatus;
}

QJsonArray AdminAccountInfo::ips() const
{
    return m_ips;
}

Identity *AdminAccountInfo::invitedByIdentity() const
{
    return m_invitedByIdentity.get();
}

Identity *AdminAccountInfo::userLevelIdentity() const
{
    return m_userLevelIdentity.get();
}

Identity *AdminAccountInfo::userLevelIdentityWithVanillaPointer() const
{
    return m_userLevelIdentityWithVanillaPointer;
}

bool AdminAccountInfo::suspended() const
{
    return m_suspended;
}

void AdminAccountInfo::setSuspended(bool suspended)
{
    if (m_suspended == suspended) {
        return;
    }
    m_suspended = suspended;
    Q_EMIT adminAccountInfoUpdated();
}

bool AdminAccountInfo::silenced() const
{
    return m_silenced;
}

void AdminAccountInfo::setSilence(bool silenced)
{
    if (m_silenced == silenced) {
        return;
    }
    m_silenced = silenced;
    Q_EMIT adminAccountInfoUpdated();
}

bool AdminAccountInfo::sensitized() const
{
    return m_sensitized;
}

void AdminAccountInfo::setSensitized(bool sensitized)
{
    if (m_sensitized == sensitized) {
        return;
    }
    m_sensitized = sensitized;
    Q_EMIT adminAccountInfoUpdated();
}

bool AdminAccountInfo::disabled() const
{
    return m_disabled;
}

void AdminAccountInfo::setDisabled(bool disabled)
{
    if (m_disabled == disabled) {
        return;
    }
    m_disabled = disabled;
    Q_EMIT adminAccountInfoUpdated();
}

bool AdminAccountInfo::approved() const
{
    return m_approved;
}

void AdminAccountInfo::setApproved(bool approved)
{
    if (m_approved == approved) {
        return;
    }
    m_approved = approved;
    Q_EMIT adminAccountInfoUpdated();
}

bool AdminAccountInfo::isLocal() const
{
    // hack to determine if an account is local
    return m_ip != QStringLiteral("");
}

int AdminAccountInfo::position() const
{
    return m_position;
}

void AdminAccountInfo::reparentAdminAccountInfo(AbstractAccount *parent)
{
    m_parent = parent;
}

void AdminAccountInfo::fromSourceData(const QJsonObject &jdoc)
{
    auto account = AccountManager::instance().selectedAccount();
    const auto doc = jdoc["account"_L1];
    m_userLevelIdentity = account->identityLookup(doc["id"_L1].toString(), doc.toObject());

    m_role = jdoc["role"_L1]["name"_L1].toString();
    m_ip = jdoc["ip"_L1].toString();
    m_ips = jdoc["ips"_L1].toArray();
    m_email = jdoc["email"_L1].toString();
    m_inviteRequest = jdoc["invite_request"_L1].toString();
    m_emailStatus = jdoc["confirmed"_L1].toBool();
    m_suspended = jdoc["suspended"_L1].toBool();
    m_silenced = jdoc["silenced"_L1].toBool();
    m_sensitized = jdoc["sensitized"_L1].toBool();
    m_disabled = jdoc["disabled"_L1].toBool();
    m_approved = jdoc["approved"_L1].toBool();
    m_locale = jdoc["locale"_L1].toString();
    m_position = jdoc["role"_L1]["position"_L1].toInt();
    m_joined = QDateTime::fromString(jdoc["created_at"_L1].toString(), Qt::ISODate).toLocalTime();

    // logic for last used activity
    const QJsonArray ipsArray = jdoc["ips"_L1].toArray();
    calculateRecentActivity(ipsArray);

    if (jdoc["email"_L1].toString().length() > 0) {
        m_emailProvider = jdoc["email"_L1].toString().split('@'_L1).at(1);
    }

    auto invited_by_account_id = jdoc["invited_by_account_id"_L1].toString();

    if (invited_by_account_id != QStringLiteral("")) {
        QUrl url = account->apiUrl(QStringLiteral("/api/v1/accounts/%1").arg(invited_by_account_id));
        account->get(url, true, this, [this, account, invited_by_account_id](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll()).object();
            m_invitedByIdentity = account->identityLookup(invited_by_account_id, doc);
        });
    }

    Q_EMIT adminAccountInfoUpdated();
}

QString AdminAccountInfo::ip() const
{
    return m_ip;
}

QString AdminAccountInfo::email() const
{
    return m_email;
}

QString AdminAccountInfo::emailProvider() const
{
    return m_emailProvider;
}

QString AdminAccountInfo::locale() const
{
    return m_locale;
}

QDateTime AdminAccountInfo::joined() const
{
    return m_joined;
}

QDateTime AdminAccountInfo::lastActive() const
{
    return m_lastActive;
}

void AdminAccountInfo::calculateRecentActivity(const QJsonArray &ipsArray)
{
    QDateTime latestDateTime;
    for (const auto &ipValue : ipsArray) {
        const auto object = ipValue.toObject();
        const QDateTime usedAtTime = QDateTime::fromString(object[QStringLiteral("used_at")].toString(), Qt::ISODate);
        if (usedAtTime > latestDateTime)
            latestDateTime = usedAtTime;
    }
    m_lastActive = latestDateTime.toLocalTime();
}

#include "moc_adminaccountinfo.cpp"