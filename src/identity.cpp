// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "identity.h"
#include "abstractaccount.h"
#include "relationship.h"
#include <QJsonObject>

QString Identity::displayName() const
{
    return !m_display_name.isEmpty() ? m_display_name : m_acct;
}

QUrl Identity::avatarUrl() const
{
    return m_avatarUrl;
}

Relationship *Identity::relationship() const
{
    return m_relationship;
}

void Identity::setRelationship(Relationship *r)
{
    if (m_relationship == r) {
        return;
    }
    if (m_relationship != nullptr) {
        // delete old relationship object if we receive a new one
        delete m_relationship;
    }
    m_relationship = r;
    Q_EMIT relationshipChanged();
}

void Identity::reparentIdentity(AbstractAccount *parent)
{
    m_parent = parent;
}

void Identity::fromSourceData(const QJsonObject &doc)
{
    m_id = doc["id"].toString().toULongLong();
    m_display_name = doc["display_name"].toString();
    m_acct = doc["acct"].toString();
    m_bio = doc["note"].toString();
    m_locked = doc["locked"].toBool();
    m_backgroundUrl = QUrl(doc["header"].toString());
    m_followersCount = doc["followers_count"].toInt();
    m_followingCount = doc["following_count"].toInt();
    m_statusesCount = doc["statuses_count"].toInt();
    m_fields = doc["fields"].toArray();

    // When the user data is ourselves, we get source.privacy
    // with the default post privacy setting for the user. all others
    // will get empty strings.
    QJsonObject source = doc["source"].toObject();
    m_visibility = source["privacy"].toString();

    m_avatarUrl = QUrl(doc["avatar"].toString());

    if (m_acct == m_parent->identity().m_acct) {
        m_parent->setDirtyIdentity();
    }

    m_displayNameHtml = m_display_name
        .replace(QLatin1Char('<'), QStringLiteral("&lt;"))
        .replace(QLatin1Char('>'), QStringLiteral("&gt;"));

    const auto emojis = doc["emojis"].toArray();

    for (const auto &emoji : emojis) {
        const auto emojiObj = emoji.toObject();
        m_displayNameHtml = m_displayNameHtml.replace(QLatin1Char(':') + emojiObj["shortcode"].toString() + QLatin1Char(':'), "<img height=\"16\" width=\"16\" src=\"" + emojiObj["static_url"].toString() + "\">");
    }
}

qint64 Identity::id() const
{
    return m_id;
}

QString Identity::displayNameHtml() const
{
    return m_displayNameHtml;
}
