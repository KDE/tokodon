// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "account/notificationfilteringpolicy.h"

#include "account/abstractaccount.h"

NotificationFilteringPolicy::NotificationFilteringPolicy(AbstractAccount *account)
    : QObject(account)
    , m_account(account)
{
    connect(account, &AbstractAccount::authenticated, this, [this, account]() {
        account->get(account->apiUrl(QStringLiteral("/api/v2/notifications/policy")), true, this, [this](QNetworkReply *reply) {
            const auto obj = QJsonDocument::fromJson(reply->readAll()).object();

            m_forNotFollowing = obj[QStringLiteral("for_not_following")].toString();
            m_forNotFollowers = obj[QStringLiteral("for_not_followers")].toString();
            m_forNewAccounts = obj[QStringLiteral("for_new_accounts")].toString();
            m_forPrivateMentions = obj[QStringLiteral("for_private_mentions")].toString();
            m_forLimitedAccounts = obj[QStringLiteral("for_limited_accounts")].toString();
            Q_EMIT forNotFollowingChanged();
            Q_EMIT forNotFollowersChanged();
            Q_EMIT forNewAccountsChanged();
            Q_EMIT forPrivateMentionsChanged();
            Q_EMIT forLimitedAccountsChanged();
        });
    });
}

QString NotificationFilteringPolicy::forNotFollowing() const
{
    return m_forNotFollowing;
}

void NotificationFilteringPolicy::setForNotFollowing(const QString &policy)
{
    if (policy == m_forNotFollowing) {
        return;
    }

    m_forNotFollowing = policy;
    Q_EMIT forNotFollowingChanged();

    setPreferencesField(QStringLiteral("for_not_following"), policy);
}

QString NotificationFilteringPolicy::forNotFollowers() const
{
    return m_forNotFollowers;
}

void NotificationFilteringPolicy::setForNotFollowers(const QString &policy)
{
    if (policy == m_forNotFollowers) {
        return;
    }

    m_forNotFollowers = policy;
    Q_EMIT forNotFollowersChanged();

    setPreferencesField(QStringLiteral("for_not_followers"), policy);
}

QString NotificationFilteringPolicy::forNewAccounts() const
{
    return m_forNewAccounts;
}

void NotificationFilteringPolicy::setForNewAccounts(const QString &policy)
{
    if (policy == m_forNewAccounts) {
        return;
    }

    m_forNewAccounts = policy;
    Q_EMIT forNewAccountsChanged();

    setPreferencesField(QStringLiteral("for_new_accounts"), policy);
}

QString NotificationFilteringPolicy::forPrivateMentions() const
{
    return m_forPrivateMentions;
}

void NotificationFilteringPolicy::setForPrivateMentions(const QString &policy)
{
    if (policy == m_forPrivateMentions) {
        return;
    }

    m_forPrivateMentions = policy;
    Q_EMIT forPrivateMentionsChanged();

    setPreferencesField(QStringLiteral("for_private_mentions"), policy);
}

QString NotificationFilteringPolicy::forLimitedAccounts() const
{
    return m_forLimitedAccounts;
}

void NotificationFilteringPolicy::setForLimitedAccounts(const QString &policy)
{
    if (policy == m_forLimitedAccounts) {
        return;
    }

    m_forLimitedAccounts = policy;
    Q_EMIT forLimitedAccountsChanged();

    setPreferencesField(QStringLiteral("for_limited_accounts"), policy);
}

void NotificationFilteringPolicy::setPreferencesField(const QString &name, const QString &value)
{
    const auto multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart preferencesPart;
    preferencesPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"%1\"").arg(name));
    preferencesPart.setBody(value.toUtf8());
    multiPart->append(preferencesPart);

    m_account->patch(m_account->apiUrl(QStringLiteral("/api/v2/notifications/policy")), multiPart, true, this, [](QNetworkReply *) { });
}

#include "moc_notificationfilteringpolicy.cpp"
