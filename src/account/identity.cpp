// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "identity.h"
#include "abstractaccount.h"
#include "relationship.h"

#include <QJsonObject>

QString Identity::displayName() const
{
    return !m_displayName.isEmpty() ? m_displayName : m_username;
}

QString Identity::username() const
{
    return m_username;
}

QString Identity::bio() const
{
    return m_bio;
}

QString Identity::account() const
{
    return m_account;
}

bool Identity::locked() const
{
    return m_locked;
}

QString Identity::visibility() const
{
    return m_visibility;
}

QUrl Identity::avatarUrl() const
{
    return m_avatarUrl;
}

QUrl Identity::backgroundUrl() const
{
    return m_backgroundUrl;
}

int Identity::followersCount() const
{
    return m_followersCount;
}

int Identity::followingCount() const
{
    return m_followingCount;
}

int Identity::statusesCount() const
{
    return m_statusesCount;
}

int Identity::permission() const
{
    return m_permission;
}

QJsonArray Identity::fields() const
{
    return m_fields;
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

    // delete old relationship object if we receive a new one
    delete m_relationship;

    m_relationship = r;
    Q_EMIT relationshipChanged();
}

void Identity::reparentIdentity(AbstractAccount *parent)
{
    m_parent = parent;
}

void Identity::fromSourceData(const QJsonObject &doc)
{
    m_id = doc["id"].toString();
    m_displayName = doc["display_name"].toString();
    m_username = doc["username"].toString();
    m_account = doc["acct"].toString();
    m_bio = doc["note"].toString();
    m_locked = doc["locked"].toBool();
    m_backgroundUrl = QUrl(doc["header"].toString());
    m_avatarUrl = QUrl(doc["avatar"].toString());
    m_followersCount = doc["followers_count"].toInt();
    m_followingCount = doc["following_count"].toInt();
    m_statusesCount = doc["statuses_count"].toInt();
    m_fields = doc["fields"].toArray();
    m_url = QUrl(doc["url"].toString());
    m_permission = doc["role"]["permissions"].toString().toInt();
    // When the user data is ourselves, we get source.privacy
    // with the default post privacy setting for the user. all others
    // will get empty strings.
    QJsonObject source = doc["source"].toObject();
    m_visibility = source["privacy"].toString();

    m_displayNameHtml = m_displayName.replace(QLatin1Char('<'), QStringLiteral("&lt;")).replace(QLatin1Char('>'), QStringLiteral("&gt;"));

    const auto emojis = doc["emojis"].toArray();

    for (const auto &emoji : emojis) {
        const auto emojiObj = emoji.toObject();
        m_displayNameHtml = m_displayNameHtml.replace(QLatin1Char(':') + emojiObj["shortcode"].toString() + QLatin1Char(':'),
                                                      "<img height=\"16\" align=\"middle\" width=\"16\" src=\"" + emojiObj["static_url"].toString() + "\">");
        m_bio = m_bio.replace(QLatin1Char(':') + emojiObj["shortcode"].toString() + QLatin1Char(':'),
                              "<img height=\"16\" width=\"16\" align=\"middle\" src=\"" + emojiObj["static_url"].toString() + "\">");
    }

    const QString baseUrl = m_url.toDisplayString(QUrl::RemovePath);
    ;

    // Attempt to replace the tag URLs with proper ones, although this should really be handled by the Mastodon API
    m_bio = m_bio.replace(baseUrl + QStringLiteral("/tags/"), QStringLiteral("hashtag:/"), Qt::CaseInsensitive);

    // Even worse, mentions are not given proper ids so we must figure it out on our own.
    // The account could be on a different server, so let's take advantage of web+ap and use that
    // to search for the account!
    // TODO: Mentions have a specific CSS class in the HTML, maybe we can use that instead of dirty regex?
    static QRegularExpression re(R"((?:href="?)(?:https?|ftp):\S[^"]+)");
    const auto match = re.match(m_bio);
    if (re.isValid()) {
        for (int i = 0; i <= match.lastCapturedIndex(); ++i) {
            const int start = match.capturedStart(i);
            const int length = match.capturedLength(i);
            const QString captured = match.captured(i);
            if (captured.contains('@')) {
                // The length of "href=" which is used in the regex.
                const int hrefLength = 6;
                m_bio = m_bio.replace(start + hrefLength, length - hrefLength, QStringLiteral("web+ap:/") + captured.mid(hrefLength));
            }
        }
    }

    Q_EMIT identityUpdated();
}

QString Identity::id() const
{
    return m_id;
}

QString Identity::displayNameHtml() const
{
    return !m_displayNameHtml.isEmpty() ? m_displayNameHtml : m_username;
}

QUrl Identity::url() const
{
    return m_url;
}
