// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "admin/emailinfo.h"

#include <QDateTime>
#include <QJsonObject>

using namespace Qt::Literals::StringLiterals;

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
    for (const auto &entry : history) {
        QJsonObject entryObj = entry.toObject();
        info.m_accountSignupCount += entryObj["accounts"_L1].toString().toInt();
        info.m_ipSignupCount += entryObj["uses"_L1].toString().toInt();
    }
}

EmailInfo EmailInfo::fromSourceData(const QJsonObject &doc)
{
    EmailInfo info;
    info.m_id = doc["id"_L1].toString();
    info.m_domain = doc["domain"_L1].toString();
    info.m_createdAt = QDateTime::fromString(doc["created_at"_L1].toString(), Qt::ISODate).toLocalTime();
    info.m_history = doc["history"_L1].toArray();
    info.calculateCount(info.m_history, info);

    return info;
}

#include "moc_emailinfo.cpp"