// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QDateTime>

class IpInfo
{
    Q_GADGET

public:
    enum SeverityValues { LimitSignUps, BlockSignUps, BlockAccess };

    Q_ENUM(SeverityValues)

    QString id() const;
    QString ip() const;
    void setIp(const QString &ip);
    SeverityValues severity() const;
    void setSeverity(const QString &severity);
    QString comment() const;
    void setComment(const QString &comment);
    QDateTime createdAt() const;
    QDateTime expiresAt() const;
    void setExpiredAt(int expiresAt);

    static IpInfo fromSourceData(const QJsonObject &doc);

private:
    static SeverityValues calculateSeverity(const QString &severity);
    QString m_id;
    QString m_ip;
    SeverityValues m_severity = LimitSignUps;
    QString m_comment;
    QDateTime m_createdAt;
    QDateTime m_expiresAt;
};
