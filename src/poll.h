// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QJsonObject>
#include <QDateTime>

class Poll
{
    Q_GADGET

    Q_PROPERTY(QString id READ id CONSTANT);
    Q_PROPERTY(QDateTime expiresAt READ expiresAt CONSTANT);
    Q_PROPERTY(bool expired READ expired CONSTANT);
    Q_PROPERTY(bool multiple READ multiple CONSTANT);
    Q_PROPERTY(int votesCount READ votesCount CONSTANT);
    Q_PROPERTY(int votersCount READ votersCount CONSTANT);
    Q_PROPERTY(bool voted READ voted CONSTANT);
    Q_PROPERTY(QList<int> ownVotes READ ownVotes CONSTANT);
    Q_PROPERTY(QList<QVariantMap> options READ options CONSTANT);

public:
    Poll();
    explicit Poll(const QJsonObject &json);

    QString id() const;
    QDateTime expiresAt() const;
    bool expired() const;
    bool multiple() const;
    int votesCount() const;
    int votersCount() const;
    bool voted() const;
    QList<int> ownVotes() const;
    QList<QVariantMap> options() const;

private:
    QString m_id;
    QDateTime m_expiresAt;
    bool m_expired;
    bool m_multiple;
    int m_votesCount;
    int m_votersCount;
    bool m_voted;
    QList<int> m_ownVotes;
    QList<QVariantMap> m_options;
};

Q_DECLARE_METATYPE(Poll *)