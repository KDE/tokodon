// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "admin/ipinfo.h"

#include <QDateTime>
#include <QJsonObject>

using namespace Qt::Literals::StringLiterals;

QString IpInfo::id() const
{
    return m_id;
}

QString IpInfo::ip() const
{
    return m_ip;
}

void IpInfo::setIp(const QString &ip)
{
    m_ip = ip;
}

IpInfo::SeverityValues IpInfo::severity() const
{
    return m_severity;
}

void IpInfo::setSeverity(const QString &severity)
{
    const SeverityValues &newCalculatedSaverity = calculateSeverity(severity);
    m_severity = newCalculatedSaverity;
}

QString IpInfo::comment() const
{
    return m_comment;
}

void IpInfo::setComment(const QString &comment)
{
    m_comment = comment;
}

QDateTime IpInfo::createdAt() const
{
    return m_createdAt;
}

QDateTime IpInfo::expiresAt() const
{
    return m_expiresAt;
}

void IpInfo::setExpiredAt(const int expiresAt)
{
    const QDateTime expiryTime = QDateTime::currentDateTime().addSecs(expiresAt);
    m_expiresAt = expiryTime;
}

IpInfo::SeverityValues IpInfo::calculateSeverity(const QString &severity)
{
    if (severity == QStringLiteral("sign_up_requires_approval")) {
        return LimitSignUps;
    } else if (severity == QStringLiteral("sign_up_block")) {
        return BlockSignUps;
    } else {
        return BlockAccess;
    }
}

IpInfo IpInfo::fromSourceData(const QJsonObject &doc)
{
    IpInfo info;
    info.m_id = doc["id"_L1].toString();
    info.m_ip = doc["ip"_L1].toString();
    info.m_severity = calculateSeverity(doc["severity"_L1].toString());
    info.m_comment = doc["comment"_L1].toString();
    info.m_createdAt = QDateTime::fromString(doc["created_at"_L1].toString(), Qt::ISODate).toLocalTime();
    info.m_expiresAt = QDateTime::fromString(doc["expires_at"_L1].toString(), Qt::ISODate).toLocalTime();
    return info;
}
