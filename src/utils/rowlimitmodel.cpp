/*
 *   Copyright 2010 by Marco Martin <mart@kde.org>
 *   Copyright 2019 by David Edmundson <davidedmundson@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "rowlimitmodel.h"

#include <QQmlContext>
#include <QQmlEngine>

RowLimitModel::RowLimitModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

RowLimitModel::~RowLimitModel() = default;

void RowLimitModel::setSourceModel(QAbstractItemModel *model)
{
    const auto oldModel = sourceModel();

    if (model == oldModel) {
        return;
    }

    QSortFilterProxyModel::setSourceModel(model);
}

int RowLimitModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 15;
}

#include "moc_rowlimitmodel.cpp"
