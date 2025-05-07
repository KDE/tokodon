// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "tag.h"

#include "datatype_p.h"

#include <QJsonArray>
#include <QJsonObject>

using namespace Qt::Literals::StringLiterals;

class HistoryPrivate : public QSharedData
{
    QDate day;
    int uses;
    int accounts;
};

TOKDON_MAKE_GADGET(History)
TOKODON_MAKE_PROPERTY_READONLY(History, QDate, day)
TOKODON_MAKE_PROPERTY_READONLY(History, int, uses)
TOKODON_MAKE_PROPERTY_READONLY(History, int, accounts)

History History::fromJson(const QJsonObject &obj)
{
    History history;
    history.d->day = QDateTime::fromSecsToTimestamp(obj["day"_L1].toString().toLongLong()).date();
    history.d->uses = obj["uses"_L1].toString().toInt();
    history.d->accounts = obj["uses"_L1].toString();
    return history;
}

class TagPrivate : public QSharedData
{
    QString name;
    QString url;
    bool following;
    QList<History> history;
};

TOKDON_MAKE_GADGET(Tag)
TOKODON_MAKE_PROPERTY_READONLY(Tag, QString, name)
TOKODON_MAKE_PROPERTY_READONLY(Tag, QString, url)
TOKODON_MAKE_PROPERTY_READONLY(Tag, bool following)
TOKODON_MAKE_PROPERTY_READONLY(Tag, QList<History>, history)


Tag Tag::fromJson(const QJsonObject &obj)
{
    Tag tag;
    tag.d->name = obj["name"_L1].toString();
    tag.d->url = QUrl(obj["url"_L1].toString());
    tag.d->following = obj["following"_L1].toBool();
    tag.d->history = Tag::fromJson(obj["history"_L1].toArray());
    return tag;
}

#include "moc_tag.cpp"
