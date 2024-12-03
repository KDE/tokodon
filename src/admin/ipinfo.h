// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QDateTime>
#include <QtQml>

class IpInfo
{
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("Only for enums")

public:
    enum SeverityValues { LimitSignUps, BlockSignUps, BlockAccess };

    Q_ENUM(SeverityValues)

    [[nodiscard]] QString id() const;
    [[nodiscard]] QString ip() const;
    void setIp(const QString &ip);
    [[nodiscard]] SeverityValues severity() const;
    void setSeverity(const QString &severity);
    [[nodiscard]] QString comment() const;
    void setComment(const QString &comment);
    [[nodiscard]] QDateTime createdAt() const;
    [[nodiscard]] QDateTime expiresAt() const;
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
