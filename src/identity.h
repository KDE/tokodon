// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonArray>
#include <QObject>
#include <QSettings>
#include <QUrl>

class AbstractAccount;
class Relationship;

class Identity : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString displayName MEMBER m_display_name CONSTANT)
    Q_PROPERTY(QString displayNameHtml READ displayNameHtml CONSTANT)
    Q_PROPERTY(QString bio MEMBER m_bio CONSTANT)
    Q_PROPERTY(QString account MEMBER m_acct CONSTANT)
    Q_PROPERTY(bool locked MEMBER m_locked CONSTANT)
    Q_PROPERTY(QString visibility MEMBER m_visibility CONSTANT)
    Q_PROPERTY(QUrl avatarUrl MEMBER m_avatarUrl CONSTANT)
    Q_PROPERTY(QUrl backgroundUrl MEMBER m_backgroundUrl CONSTANT)
    Q_PROPERTY(int followersCount MEMBER m_followersCount CONSTANT)
    Q_PROPERTY(int followingCount MEMBER m_followingCount CONSTANT)
    Q_PROPERTY(int statusesCount MEMBER m_statusesCount CONSTANT)
    Q_PROPERTY(QJsonArray fields MEMBER m_fields CONSTANT)

    Q_PROPERTY(Relationship *relationship READ relationship NOTIFY relationshipChanged)

public:
    qint64 id() const;

    qint64 m_id;
    QString m_display_name;
    QString m_bio;
    QString m_acct;
    bool m_locked;
    QString m_visibility;
    QUrl m_avatarUrl;
    QUrl m_backgroundUrl;
    QJsonArray m_fields;
    int m_followersCount;
    int m_followingCount;
    int m_statusesCount;
    void fromSourceData(const QJsonObject &doc);
    void fetchAvatar(const QUrl &avatar_url);
    void reparentIdentity(AbstractAccount *parent);

    AbstractAccount *m_parent = nullptr;

    Relationship *relationship() const;
    void setRelationship(Relationship *r);

    QUrl avatarUrl() const;
    QString displayName() const;
    QString displayNameHtml() const;

Q_SIGNALS:
    void relationshipChanged();

private:
    QString m_displayNameHtml;
    Relationship *m_relationship = nullptr;
};
