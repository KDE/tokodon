// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "poll.h"

#include <QJsonObject>

#include "utils/customemoji.h"
#include "utils/texthandler.h"

using namespace Qt::Literals::StringLiterals;

class PollPrivate : public QSharedData
{
public:
    QString id;
    QDateTime expiresAt;
    bool expired = false;
    bool multiple = false;
    int votesCount = 0;
    int votersCount = 0;
    bool voted = false;
    QList<int> ownVotes;
    QList<QVariantMap> options;
};

Poll Poll::fromJson(const QJsonObject &json)
{
    Poll poll;
    poll.d->id = json["id"_L1].toString();
    poll.d->expiresAt = QDateTime::fromString(json["expires_at"_L1].toString(), Qt::ISODate);
    poll.d->expired = json["expired"_L1].toBool();
    poll.d->multiple = json["multiple"_L1].toBool();
    poll.d->votesCount = json["votes_count"_L1].toInt(-1);
    poll.d->votersCount = json["voters_count"_L1].toInt(-1);
    poll.d->voted = json["voted"_L1].toBool();
    const auto ownVotes = json["own_votes"_L1].toArray();
    std::ranges::transform(std::as_const(ownVotes), std::back_inserter(poll.d->ownVotes), [](const QJsonValue &value) -> auto {
        return value.toInt();
    });

    const auto emojis = CustomEmoji::parseCustomEmojis(json["emojis"_L1].toArray());

    const auto options = json["options"_L1].toArray();
    std::ranges::transform(std::as_const(options), std::back_inserter(poll.d->options), [emojis](const QJsonValue &value) -> QVariantMap {
        const auto option = value.toObject();
        const QString title = TextHandler::replaceCustomEmojis(emojis, option["title"_L1].toString());

        return {
            {"title"_L1, title},
            {"votesCount"_L1, option["votes_count"_L1].toInt(-1)},
        };
    });
    return poll;
}

TOKODON_MAKE_PROPERTY(Poll, QString, id, setId)
TOKODON_MAKE_PROPERTY(Poll, QDateTime, expiresAt, setExpiresAt)
TOKODON_MAKE_PROPERTY(Poll, bool, expired, setExpired)
TOKODON_MAKE_PROPERTY(Poll, bool, multiple, setMultime)
TOKODON_MAKE_PROPERTY(Poll, int, votesCount, setVotesCount)
TOKODON_MAKE_PROPERTY(Poll, int, votersCount, setvotersCount)
TOKODON_MAKE_PROPERTY(Poll, bool, voted, setVoted)
TOKODON_MAKE_PROPERTY(Poll, QList<int>, ownVotes, setOwnVotes)
TOKODON_MAKE_PROPERTY(Poll, QList<QVariantMap>, options, setOptions)

bool Poll::isNull() const
{
    return m_id.isEmpty();
}

#include "moc_poll.cpp"
