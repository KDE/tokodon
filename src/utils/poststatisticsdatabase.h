// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDateTime>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <qqmlintegration.h>

class Post;
class AbstractAccount;

class PostStatisticsDatabase : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    explicit PostStatisticsDatabase(QObject *parent = nullptr);

    Q_INVOKABLE void fill(AbstractAccount *account);

    bool loading() const;

Q_SIGNALS:
    void loadingChanged();

private:
    struct PostStatistic {
        QString id;
        QString content;
        int favoriteCount, boostCount, replyCount;
        QDateTime publishedAt;
    };

    void setLoading(bool loading);

    QSqlDatabase m_db;
    QString m_lastId;
    bool m_loading = false;
};