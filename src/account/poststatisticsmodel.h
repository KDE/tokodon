// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtQml>

#include "utils/poststatisticsdatabase.h"

/**
 * @brief Fetches server announcements.
 */
class PostStatisticsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(PostStatisticsDatabase *database READ database WRITE setDatabase NOTIFY databaseChanged)

public:
    /**
     * @brief Custom roles for this model.
     */
    enum CustomRoles { IdRole = Qt::UserRole, ContentRole, FavoriteCountRole, BoostCountRole, ReplyCountRole, PublishedAtRole };

    explicit PostStatisticsModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setDatabase(PostStatisticsDatabase *database);
    PostStatisticsDatabase *database() const;

Q_SIGNALS:
    void loadingChanged();
    void databaseChanged();

private:
    void fill();

    struct PostStatistic {
        QString id;
        QString content;
        int favoriteCount, boostCount, replyCount;
        QDateTime publishedAt;
    };

    PostStatisticsDatabase *m_database;
    QList<PostStatistic> m_postStatistics;
    bool m_loading = false;
};
