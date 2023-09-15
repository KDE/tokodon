// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDateTime>
#include <QJsonArray>
#include <QObject>

class EmailInfo
{
    Q_GADGET

public:
    enum SeverityValues { LimitSignUps, BlockSignUps, BlockAccess };

    Q_ENUM(SeverityValues)

    QString id() const;
    QString domain() const;
    QDateTime createdAt() const;
    int accountSignupCount() const;
    int ipSignupCount() const;

    void calculateCount(const QJsonArray &history, EmailInfo &info);

    static EmailInfo fromSourceData(const QJsonObject &doc);

private:
    int m_accountSignupCount = 0;
    int m_ipSignupCount = 0;
    QString m_id;
    QString m_domain;
    QDateTime m_createdAt;
    QJsonArray m_history;
};
