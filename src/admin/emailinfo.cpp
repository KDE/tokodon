// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "admin/emailinfo.h"

#include <QDateTime>
#include <QJsonObject>

QString EmailInfo::id() const
{
    return m_id;
}

QString EmailInfo::domain() const
{
    return m_domain;
}

QDateTime EmailInfo::createdAt() const
{
    return m_createdAt;
}

int EmailInfo::accountSignupCount() const
{
    return m_accountSignupCount;
}

int EmailInfo::ipSignupCount() const
{
    return m_ipSignupCount;
}

void EmailInfo::calculateCount(const QJsonArray &history, EmailInfo &info)
{
    for (const QJsonValue &entry : history) {
        QJsonObject entryObj = entry.toObject();
        info.m_accountSignupCount += entryObj["accounts"].toString().toInt();
        info.m_ipSignupCount += entryObj["uses"].toString().toInt();
    }
}

EmailInfo EmailInfo::fromSourceData(const QJsonObject &doc)
{
    EmailInfo info;
    info.m_id = doc["id"].toString();
    info.m_domain = doc["domain"].toString();
    info.m_createdAt = QDateTime::fromString(doc["created_at"].toString(), Qt::ISODate).toLocalTime();
    info.m_history = doc["history"].toArray();
    info.calculateCount(info.m_history, info);

    return info;
}
