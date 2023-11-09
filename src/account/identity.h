// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonArray>

class AbstractAccount;
class Relationship;

/// Represents a profile on the server. These are attached to Posts, and even our own Accounts
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
    /// The numeric ID associated with this identity
    QString id() const;

    /// This identity's display name, if not set then returns the username
    QString displayName() const;

    /// The username for this identity
    QString username() const;

    /// This identity's display name, but processed as HTML (for custom emojis). If not set, returns the username
    QString displayNameHtml() const;

    /// The biography for this identity
    QString bio() const;

    /// The account ID for this identity
    QString account() const;

    /// The URL to this identity's profile, which exists on their original server
    QUrl url() const;

    /// If this identity is locked or not
    bool locked() const;

    /// The profile visibility
    QString visibility() const;

    /// URL to this identity's avatar
    QUrl avatarUrl() const;

    /// URL to this identity's background
    QUrl backgroundUrl() const;

    /// The number of users following this identity
    int followersCount() const;

    /// The number of users this identity follows
    int followingCount() const;

    /// The number of posts written by this identity
    int statusesCount() const;

    /// The permissions of this identity
    int permission() const;

    /// Custom fields set by this identity
    QJsonArray fields() const;

    /// Fills in identity data from JSON
    /// \param doc The JSON data to load from
    void fromSourceData(const QJsonObject &doc);

    /// Sets the parent account for this identity
    /// \param parent The account to reparent to
    void reparentIdentity(AbstractAccount *parent);

    /// Returns the relationship to this identity
    Relationship *relationship() const;

    /// Replaces the existing relationship with a new one
    /// \param r The new relationship
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
    Relationship *m_relationship = nullptr;
    AbstractAccount *m_parent = nullptr;
};
