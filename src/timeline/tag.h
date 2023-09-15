// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QObject>
#include <QQmlListProperty>
#include <QString>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class History
{
    Q_GADGET

    Q_PROPERTY(QString day MEMBER m_day)
    Q_PROPERTY(QString uses MEMBER m_uses)
    Q_PROPERTY(QString accounts MEMBER m_accounts)

public:
    History() = default;
    explicit History(const QString &day, const QString &uses, const QString &accounts);

private:
    QString m_day;
    QString m_uses;
    QString m_accounts;
};

class Tag
{
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("Access via TagsModel")

    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(QUrl url MEMBER m_url)
    Q_PROPERTY(bool following MEMBER m_following)
    Q_PROPERTY(QList<History> history READ history CONSTANT)

public:
    Tag() = default;
    explicit Tag(QJsonObject obj);

    QString name() const;
    QUrl url() const;
    QList<History> history() const;

private:
    void fromJson(QJsonObject obj);

    QString m_name;
    QUrl m_url;
    bool m_following = false;
    QList<History> m_history;
};
