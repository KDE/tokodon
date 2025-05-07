// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QQmlEngine>
#include <QExplicitlySharedDataPointer>
#include "datatype.h"

class PollPrivate;

class Poll final
{
    TOKODON_GADGET(Poll, poll)

    TOKODON_PROPERTY(QString, id, setId)
    TOKODON_PROPERTY(QDateTime, expiresAt, setExpiresAt)
    TOKODON_PROPERTY(bool, expired, setExpired)
    TOKODON_PROPERTY(bool, multiple, setMultime)
    TOKODON_PROPERTY(int, votesCount, setVotesCount)
    TOKODON_PROPERTY(int, votersCount, setvotersCount)
    TOKODON_PROPERTY(bool, voted, setVoted)
    TOKODON_PROPERTY(QList<int>, ownVotes, setOwnVotes)
    TOKODON_PROPERTY(QList<QVariantMap>, options, setOptions)
    Q_PROPERTY(bool isNull READ isNull)

public:
    [[nodiscard]] bool isNull() const;
};
