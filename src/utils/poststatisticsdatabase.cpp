// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils/poststatisticsdatabase.h"

#include <QFile>
#include <QSqlDriver>
#include <QSqlError>
#include <QUrlQuery>

#include "account/account.h"

using namespace Qt::StringLiterals;

PostStatisticsDatabase::PostStatisticsDatabase(QObject *parent)
    : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    m_db.setDatabaseName(QStringLiteral("customdb.db"));

    if (!m_db.open()) {
        qFatal() << "Failed to open custom db!";
    }

    QSqlQuery query;
    query.exec(
        QStringLiteral("CREATE TABLE IF NOT EXISTS posts (id TEXT PRIMARY KEY, content TEXT NOT NULL, favorite_count INTEGER NOT NULL, boost_count INTEGER NOT "
                       "NULL, reply_count INTEGER NOT NULL, created_at TEXT NOT NULL)"));
}

void PostStatisticsDatabase::fill(AbstractAccount *account)
{
    setLoading(true);

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("limit"), QString::number(40));
    query.addQueryItem(QStringLiteral("max_id"), m_lastId);
    query.addQueryItem(QStringLiteral("exclude_reblogs"), QStringLiteral("true"));

    QUrl url = account->apiUrl(QStringLiteral("/api/v1/accounts/%1/statuses").arg(account->identity()->id()));
    url.setQuery(query);

    account->get(url, true, this, [this, account](QNetworkReply *reply) {
        qInfo() << reply;

        const auto doc = QJsonDocument::fromJson(reply->readAll());
        auto statuses = doc.array().toVariantList();

        if (!statuses.isEmpty()) {
            for (const QVariant &var : statuses) {
                const auto &postObj = var.toJsonObject();

                const auto &id = postObj["id"_L1].toString();
                const auto &content = postObj["content"_L1].toString();
                const auto &boostCount = postObj["reblogs_count"_L1].toInt();
                const auto &favoriteCount = postObj["favourites_count"_L1].toInt();
                const auto &replyCount = postObj["reply_count"_L1].toInt();
                const auto &publishedAt = postObj["created_at"_L1].toString();

                QSqlQuery query;
                query.prepare(
                    QStringLiteral("REPLACE INTO posts (id, content, favorite_count, boost_count, reply_count, created_at) "
                                   "VALUES (?, ?, ?, ?, ?, ?)"));
                query.addBindValue(id);
                query.addBindValue(content);
                query.addBindValue(favoriteCount);
                query.addBindValue(boostCount);
                query.addBindValue(replyCount);
                query.addBindValue(publishedAt);
                query.exec();

                qInfo() << query.lastError();
            }

            m_lastId = statuses.last().toJsonObject()[QLatin1String("id")].toString();

            // fill next page
            fill(account);
        } else {
            setLoading(false);
        }
    });
}

bool PostStatisticsDatabase::loading() const
{
    return m_loading;
}

void PostStatisticsDatabase::setLoading(const bool loading)
{
    if (m_loading != loading) {
        m_loading = loading;
        Q_EMIT loadingChanged();
    }
}

#include "moc_poststatisticsdatabase.cpp"