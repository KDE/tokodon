// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "poll.h"

#include <QJsonArray>
#include <algorithm>

using namespace Qt::Literals::StringLiterals;

Poll::Poll() = default;

Poll::Poll(const QJsonObject &json)
{
    m_id = json[QStringLiteral("id")].toString();
    m_expiresAt = QDateTime::fromString(json[QStringLiteral("expires_at")].toString(), Qt::ISODate);
    m_expired = json[QStringLiteral("expired")].toBool();
    m_multiple = json[QStringLiteral("multiple")].toBool();
    m_votesCount = json[QStringLiteral("votes_count")].toInt(-1);
    m_votersCount = json[QStringLiteral("voters_count")].toInt(-1);
    m_voted = json[QStringLiteral("voted")].toBool();
    const auto ownVotes = json[QStringLiteral("own_votes")].toArray();
    std::transform(
        ownVotes.cbegin(),
        ownVotes.cend(),
        std::back_inserter(m_ownVotes),
        [](const QJsonValue &value) -> auto{ return value.toInt(); });

    const auto emojis = json[QStringLiteral("emojis")].toArray();

    const auto options = json[QStringLiteral("options")].toArray();
    std::transform(options.cbegin(), options.cend(), std::back_inserter(m_options), [emojis](const QJsonValue &value) -> QVariantMap {
        const auto option = value.toObject();
        QString title = option[QStringLiteral("title")].toString();
        for (const auto &emoji : emojis) {
            const auto emojiObj = emoji.toObject();
            title = title.replace(QLatin1Char(':') + emojiObj["shortcode"_L1].toString() + QLatin1Char(':'),
                                  QStringLiteral("<img height=\"16\" align=\"middle\" width=\"16\" src=\"") + emojiObj["static_url"_L1].toString()
                                      + QStringLiteral("\">"));
        }
        return {
            {"title"_L1, title},
            {"votesCount"_L1, option[QStringLiteral("votes_count")].toInt(-1)},
        };
    });
}

QString Poll::id() const
{
    return m_id;
}

QDateTime Poll::expiresAt() const
{
    return m_expiresAt;
}

bool Poll::expired() const
{
    return m_expired;
}

bool Poll::multiple() const
{
    return m_multiple;
}

int Poll::votesCount() const
{
    return m_votesCount;
}

int Poll::votersCount() const
{
    return m_votersCount;
}

bool Poll::voted() const
{
    return m_voted;
}

QList<int> Poll::ownVotes() const
{
    return m_ownVotes;
}

QList<QVariantMap> Poll::options() const
{
    return m_options;
}
