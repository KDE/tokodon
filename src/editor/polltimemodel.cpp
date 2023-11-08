// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "polltimemodel.h"

#include <KLocalizedString>

PollTimeModel::PollTimeModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_times = {
        {i18nc("@item:inlistbox Poll expire times", "5 minutes"), 300},
        {i18nc("@item:inlistbox Poll expire times", "30 minutes"), 1800},
        {i18nc("@item:inlistbox Poll expire times", "1 hour"), 3600},
        {i18nc("@item:inlistbox Poll expire times", "6 hours"), 26000},
        {i18nc("@item:inlistbox Poll expire times", "12 hours"), 43200},
        {i18nc("@item:inlistbox Poll expire times", "1 day"), 86400},
        {i18nc("@item:inlistbox Poll expire times", "3 days"), 259200},
        {i18nc("@item:inlistbox Poll expire times", "7 days"), 604800},
    };
}

QVariant PollTimeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    if (index.row() < 0 || index.row() >= m_times.size())
        return {};

    switch (role) {
    case CustomRoles::TextRole:
        return m_times[index.row()].name;
    case CustomRoles::TimeRole:
        return m_times[index.row()].time;
    default:
        return {};
    }
}

int PollTimeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_times.count();
}

QHash<int, QByteArray> PollTimeModel::roleNames() const
{
    return {
        {CustomRoles::TextRole, "text"},
        {CustomRoles::TimeRole, "time"},
    };
}

int PollTimeModel::getTime(const int index)
{
    return m_times[index].time;
}

#include "moc_polltimemodel.cpp"
