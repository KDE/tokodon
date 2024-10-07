// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonArray>

class AbstractAccount;
class Relationship;

/**
 * @brief Represents a profile on the server.
 *
 * These are attached to Posts, and even our own Accounts.
 */
class Identity : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id NOTIFY identityUpdated)
    Q_PROPERTY(QString displayName READ displayName NOTIFY identityUpdated)
    Q_PROPERTY(QString displayNameHtml READ displayNameHtml NOTIFY identityUpdated)
    Q_PROPERTY(QString username READ username NOTIFY identityUpdated)
    Q_PROPERTY(QString bio READ bio NOTIFY identityUpdated)
    Q_PROPERTY(QString account READ account NOTIFY identityUpdated)
    Q_PROPERTY(QUrl url READ url NOTIFY identityUpdated)
    Q_PROPERTY(bool locked READ locked NOTIFY identityUpdated)
    Q_PROPERTY(QString visibility READ visibility NOTIFY identityUpdated)
    Q_PROPERTY(QUrl avatarUrl READ avatarUrl NOTIFY identityUpdated)
    Q_PROPERTY(QUrl backgroundUrl READ backgroundUrl NOTIFY identityUpdated)
    Q_PROPERTY(int followersCount READ followersCount NOTIFY identityUpdated)
    Q_PROPERTY(int followingCount READ followingCount NOTIFY identityUpdated)
    Q_PROPERTY(int statusesCount READ statusesCount NOTIFY identityUpdated)
    Q_PROPERTY(int permission READ permission NOTIFY identityUpdated)
    Q_PROPERTY(QJsonArray fields READ fields NOTIFY identityUpdated)
    Q_PROPERTY(Relationship *relationship READ relationship NOTIFY relationshipChanged)

public:
    /**
     * @return The numeric ID associated with this identity.
     */
    [[nodiscard]] QString id() const;

    /**
     * @return This identity's display name, if not set then returns the username
     */
    [[nodiscard]] QString displayName() const;

    /**
     * @return The username for this identity.
     */
    [[nodiscard]] QString username() const;

    /**
     * @return This identity's display name, but processed as HTML (for custom emojis). If not set, returns the username.
     */
    [[nodiscard]] QString displayNameHtml() const;

    /**
     * @return The biography for this identity.
     */
    [[nodiscard]] QString bio() const;

    /**
     * @return The account ID for this identity.
     */
    [[nodiscard]] QString account() const;

    /**
     * @return The URL to this identity's profile, which exists on their original server.
     */
    [[nodiscard]] QUrl url() const;

    /**
     * @return If this identity is locked or not.
     */
    [[nodiscard]] bool locked() const;

    /**
     * @return The profile visibility.
     */
    [[nodiscard]] QString visibility() const;

    /**
     * @return URL to this identity's avatar.
     */
    [[nodiscard]] QUrl avatarUrl() const;

    /**
     * @return URL to this identity's background.
     */
    [[nodiscard]] QUrl backgroundUrl() const;

    /**
     * @return The number of users following this identity.
     */
    [[nodiscard]] int followersCount() const;

    /**
     * @return The number of users this identity follows.
     */
    [[nodiscard]] int followingCount() const;

    /**
     * @return The number of posts written by this identity.
     */
    [[nodiscard]] int statusesCount() const;

    /**
     * @return The permissions of this identity.
     */
    [[nodiscard]] int permission() const;

    /**
     * @return Custom fields set by this identity.
     */
    [[nodiscard]] QJsonArray fields() const;

    [[nodiscard]] QDate lastStatusAt() const;

    /**
     * @brief Fills in identity data from JSON.
     * @param doc The JSON data to load from.
     */
    void fromSourceData(const QJsonObject &doc);

    /**
     * @brief Sets the parent account for this identity.
     * @param parent The account to reparent to.
     */
    void reparentIdentity(AbstractAccount *parent);

    /**
     * @return Our relationship to this identity.
     */
    [[nodiscard]] Relationship *relationship() const;

    /**
     * @brief Replaces the existing relationship with a new one.
     * @param r The new relationship.
     */
    void setRelationship(Relationship *r);

Q_SIGNALS:
    void relationshipChanged();
    void identityUpdated();

private:
    QString m_id;
    QString m_displayName;
    QString m_displayNameHtml;
    QString m_username;
    QString m_bio;
    QString m_account;
    bool m_locked;
    QString m_visibility;
    QUrl m_avatarUrl;
    QUrl m_backgroundUrl;
    QUrl m_url;
    QJsonArray m_fields;
    int m_followersCount;
    int m_followingCount;
    int m_statusesCount;
    int m_permission;
    QDate m_lastStatusAt;
    Relationship *m_relationship = nullptr;
    AbstractAccount *m_parent = nullptr;
};
