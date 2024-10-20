// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "utils/limitermodel.h"

LimiterModel::LimiterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, &QSortFilterProxyModel::rowsInserted, this, [this] {
        const int extraCount = std::max(sourceModel()->rowCount() - maximumCount(), 0);
        if (extraCount != m_extraCount) {
            m_extraCount = extraCount;
            Q_EMIT extraCountChanged();
        }
    });
}

int LimiterModel::maximumCount() const
{
    return m_maximumCount;
}

void LimiterModel::setMaximumCount(int maximumCount)
{
    if (m_maximumCount != maximumCount) {
        m_maximumCount = maximumCount;
        Q_EMIT maximumCountChanged();
    }
}

int LimiterModel::extraCount() const
{
    return m_extraCount;
}

bool LimiterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    return source_row <= maximumCount();
}

#include "moc_limitermodel.cpp"
