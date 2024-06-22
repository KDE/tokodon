// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/poststatisticsmodel.h"

#include "account/accountmanager.h"

#include <QSqlError>
#include <QSqlQuery>

using namespace Qt::Literals::StringLiterals;

PostStatisticsModel::PostStatisticsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fill();
}

QVariant PostStatisticsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &postStatistic = m_postStatistics[index.row()];

    switch (role) {
    case IdRole:
        return postStatistic.id;
    case ContentRole:
        return postStatistic.content;
    case FavoriteCountRole:
        return postStatistic.favoriteCount;
    case BoostCountRole:
        return postStatistic.boostCount;
    case ReplyCountRole:
        return postStatistic.replyCount;
    case PublishedAtRole:
        return postStatistic.publishedAt;
    default:
        return {};
    }
}

bool PostStatisticsModel::loading() const
{
    return m_loading;
}

void PostStatisticsModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int PostStatisticsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_postStatistics.size();
}

QHash<int, QByteArray> PostStatisticsModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {ContentRole, "content"},
        {FavoriteCountRole, "favoriteCount"},
        {BoostCountRole, "boostCount"},
        {ReplyCountRole, "replyCount"},
        {PublishedAtRole, "publishedAt"},
    };
}

void PostStatisticsModel::setDatabase(PostStatisticsDatabase *database)
{
    if (m_database != database) {
        m_database = database;
        setLoading(true);
        fill();
        Q_EMIT databaseChanged();
    }
}

PostStatisticsDatabase *PostStatisticsModel::database() const
{
    return m_database;
}

void PostStatisticsModel::fill()
{
    QSqlQuery query;
    query.exec(QStringLiteral("SELECT id, content, boost_count, favorite_count, reply_count, created_at FROM posts"));

    qInfo() << query.lastError();

    QVector<QString> folders;
    while (query.next()) {
        PostStatistic postStatistic;
        postStatistic.id = query.value(0).toString();
        postStatistic.content = query.value(1).toString();
        postStatistic.boostCount = query.value(2).toInt();
        postStatistic.favoriteCount = query.value(3).toInt();
        postStatistic.replyCount = query.value(4).toInt();
        // postStatistic.publishedAt = query.value(0).toString();

        beginInsertRows(QModelIndex(), m_postStatistics.size(), m_postStatistics.size());
        m_postStatistics.push_back(postStatistic);
        endInsertRows();
    }
}

#include "moc_poststatisticsmodel.cpp"