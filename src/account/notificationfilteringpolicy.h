// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>

class AbstractAccount;

/**
 * @brief Account-specific notification filtering preferences which are stored server-side.
 */
class NotificationFilteringPolicy final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString forNotFollowing READ forNotFollowing WRITE setForNotFollowing NOTIFY forNotFollowingChanged)
    Q_PROPERTY(QString forNotFollowers READ forNotFollowers WRITE setForNotFollowers NOTIFY forNotFollowersChanged)
    Q_PROPERTY(QString forNewAccounts READ forNewAccounts WRITE setForNewAccounts NOTIFY forNewAccountsChanged)
    Q_PROPERTY(QString forPrivateMentions READ forPrivateMentions WRITE setForPrivateMentions NOTIFY forPrivateMentionsChanged)
    Q_PROPERTY(QString forLimitedAccounts READ forLimitedAccounts WRITE setForLimitedAccounts NOTIFY forLimitedAccountsChanged)

public:
    explicit NotificationFilteringPolicy(AbstractAccount *account);

    /**
     * @return Whether to accept, filter or drop notifications from accounts the user is not following.
     * @see setForNotFollowing()
     */
    [[nodiscard]] QString forNotFollowing() const;

    /**
     * @brief Sets whether to accept, filter or drop notifications from accounts the user is not following.
     * @param policy The new policy, which can either be `accept` `filter` or `drop`.
     * @see forNotFollowing()
     */
    void setForNotFollowing(const QString &policy);

    /**
     * @return Whether to accept, filter or drop notifications from accounts that are not following the user.
     * @see setForNotFollowers()
     */
    [[nodiscard]] QString forNotFollowers() const;

    /**
     * @brief Sets whether to accept, filter or drop notifications from accounts that are not following the user.
     * @param policy The new policy, which can either be `accept` `filter` or `drop`.
     * @see forNotFollowers()
     */
    void setForNotFollowers(const QString &policy);

    /**
     * @return Whether to accept, filter or drop notifications from accounts created in the past 30 days.
     * @see setForNewAccounts()
     */
    [[nodiscard]] QString forNewAccounts() const;

    /**
     * @brief Sets whether to accept, filter or drop notifications from accounts created in the past 30 days.
     * @param policy The new policy, which can either be `accept` `filter` or `drop`.
     * @see forNewAccounts()
     */
    void setForNewAccounts(const QString &policy);

    /**
     * @return Whether to accept, filter or drop notifications from private mentions.
     * @see setForPrivateMentions()
     */
    [[nodiscard]] QString forPrivateMentions() const;

    /**
     * @brief Sets whether to accept, filter or drop notifications from private mentions.
     * @param policy The new policy, which can either be `accept` `filter` or `drop`.
     * @see forPrivateMentions()
     */
    void setForPrivateMentions(const QString &policy);

    /**
     * @brief Whether to accept, filter or drop notifications from accounts that were limited by a moderator.
     * @param policy The new policy, which can either be `accept` `filter` or `drop`.
     * @see setForLimitedAccounts()
     */
    [[nodiscard]] QString forLimitedAccounts() const;

    /**
     * @brief Sets whether to accept, filter or drop notifications from accounts that were limited by a moderator.
     * @param policy The new policy, which can either be `accept` `filter` or `drop`.
     * @see forLimitedAccounts()
     */
    void setForLimitedAccounts(const QString &policy);

Q_SIGNALS:
    /**
     * @brief Emitted when the for not following policy has changed.
     * @see setForNotFollowing()
     */
    void forNotFollowingChanged();

    /**
     * @brief Emitted when the for not followers policy has changed.
     * @see setForNotFollowers()
     */
    void forNotFollowersChanged();

    /**
     * @brief Emitted when the for new accounts policy has changed.
     * @see setForNewAccounts()
     */
    void forNewAccountsChanged();

    /**
     * @brief Emitted when the for private mentions policy has changed.
     * @see setForPrivateMentions()
     */
    void forPrivateMentionsChanged();

    /**
     * @brief Emitted when the for private mentions policy has changed.
     * @see setForPrivateMentions()
     */
    void forLimitedAccountsChanged();

private:
    void setPreferencesField(const QString &name, const QString &value);

    AbstractAccount *const m_account;

    QString m_forNotFollowing;
    QString m_forNotFollowers;
    QString m_forNewAccounts;
    QString m_forPrivateMentions;
    QString m_forLimitedAccounts;
};
