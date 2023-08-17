// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QJsonArray>
#include <QJsonObject>
#include <QUrl>

#include "tag.h"

Tag::Tag(QJsonObject obj)
{
    fromJson(obj);
}

void Tag::fromJson(QJsonObject obj)
{
    m_name = obj["name"].toString();
    m_url = QUrl(obj["url"].toString());
    m_following = obj["following"].toBool();
    const auto historyArray = obj["history"].toArray();
    for (const QJsonValue &historyValue : historyArray) {
        QJsonObject historyObj = historyValue.toObject();
        QString day = historyObj["day"].toString();
        QString uses = historyObj["uses"].toString();
        QString accounts = historyObj["accounts"].toString();
        History history(day, uses, accounts);
        m_history.append(history);
    }
}

History::History(const QString &day, const QString &uses, const QString &accounts)
    : m_day(day)
    , m_uses(uses)
    , m_accounts(accounts)
{
}

QList<History> Tag::history() const
{
    return m_history;
}

QUrl Tag::url() const
{
    return m_url;
}

QString Tag::name() const
{
    return m_name;
}
