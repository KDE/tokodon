// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QJsonArray>
#include <QJsonObject>
#include <QUrl>

#include "tag.h"

using namespace Qt::Literals::StringLiterals;

Tag::Tag(QJsonObject obj)
{
    fromJson(obj);
}

void Tag::fromJson(QJsonObject obj)
{
    m_name = obj["name"_L1].toString();
    m_url = QUrl(obj["url"_L1].toString());
    m_following = obj["following"_L1].toBool();
    const auto historyArray = obj["history"_L1].toArray();
    for (const QJsonValue &historyValue : historyArray) {
        QJsonObject historyObj = historyValue.toObject();
        QString day = historyObj["day"_L1].toString();
        QString uses = historyObj["uses"_L1].toString();
        QString accounts = historyObj["accounts"_L1].toString();
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
