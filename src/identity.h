// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonArray>
#include <QObject>
#include <QSettings>
#include <QUrl>
#include <qjsonarray.h>

class AbstractAccount;
class Relationship;

class Identity : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString displayName READ displayName NOTIFY identityUpdated)
    Q_PROPERTY(QString displayNameHtml READ displayNameHtml NOTIFY identityUpdated)
    Q_PROPERTY(QString username READ username NOTIFY identityUpdated)
    Q_PROPERTY(QString bio READ bio NOTIFY identityUpdated)
    Q_PROPERTY(QString account READ account NOTIFY identityUpdated)
    Q_PROPERTY(bool locked READ locked NOTIFY identityUpdated)
    Q_PROPERTY(QString visibility READ visibility NOTIFY identityUpdated)
    Q_PROPERTY(QUrl avatarUrl READ avatarUrl NOTIFY identityUpdated)
    Q_PROPERTY(QUrl backgroundUrl READ backgroundUrl NOTIFY identityUpdated)
    Q_PROPERTY(int followersCount READ followersCount NOTIFY identityUpdated)
    Q_PROPERTY(int followingCount READ followingCount NOTIFY identityUpdated)
    Q_PROPERTY(int statusesCount READ statusesCount NOTIFY identityUpdated)
    Q_PROPERTY(QJsonArray fields READ fields NOTIFY identityUpdated)
    Q_PROPERTY(Relationship *relationship READ relationship NOTIFY relationshipChanged)

public:
    qint64 id() const;
    QString displayName() const;
    QString username() const;
    QString displayNameHtml() const;
    QString bio() const;
    QString account() const;
    bool locked() const;
    QString visibility() const;
    QUrl avatarUrl() const;
    QUrl backgroundUrl() const;
    int followersCount() const;
    int followingCount() const;
    int statusesCount() const;
    QJsonArray fields() const;

    void fromSourceData(const QJsonObject &doc);
    void fetchAvatar(const QUrl &avatar_url);
    void reparentIdentity(AbstractAccount *parent);

    Relationship *relationship() const;
    void setRelationship(Relationship *r);

Q_SIGNALS:
    void relationshipChanged();
    void identityUpdated();

private:
    qint64 m_id;
    QString m_displayName;
    QString m_displayNameHtml;
    QString m_username;
    QString m_bio;
    QString m_account;
    bool m_locked;
    QString m_visibility;
    QUrl m_avatarUrl;
    QUrl m_backgroundUrl;
    QJsonArray m_fields;
    int m_followersCount;
    int m_followingCount;
    int m_statusesCount;
    Relationship *m_relationship = nullptr;
    AbstractAccount *m_parent = nullptr;
};
