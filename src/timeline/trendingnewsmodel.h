// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "timeline/abstractlistmodel.h"

class TrendingNewsModel : public AbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit TrendingNewsModel(QObject *parent = nullptr);
    ~TrendingNewsModel() override;

    enum CustomRoles { UrlRole = Qt::UserRole, TitleRole, DescriptionRole, ImageRole };

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] QString displayName() const override;

private:
    void fill();

    struct Link {
        QString url;
        QString title;
        QString description;
        QString image;
    };
    QList<Link> m_links;
    [[nodiscard]] Link fromSourceData(const QJsonObject &object) const;
};
