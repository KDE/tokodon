// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "timeline/poll.h"

#include <QJsonObject>

#include "utils/customemoji.h"
#include "utils/texthandler.h"

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
    std::ranges::transform(std::as_const(ownVotes), std::back_inserter(m_ownVotes), [](const QJsonValue &value) -> auto {
        return value.toInt();
    });

    const auto emojis = CustomEmoji::parseCustomEmojis(json[QStringLiteral("emojis")].toArray());

    const auto options = json[QStringLiteral("options")].toArray();
    std::ranges::transform(std::as_const(options), std::back_inserter(m_options), [emojis](const QJsonValue &value) -> QVariantMap {
        const auto option = value.toObject();
        QString title = TextHandler::replaceCustomEmojis(emojis, option[QStringLiteral("title")].toString());

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

bool Poll::hasVotedFor(const int index) const
{
    return m_ownVotes.contains(index);
}

#include "moc_poll.cpp"
