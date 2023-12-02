// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "polltimemodel.h"

#include <KLocalizedString>

PollTimeModel::PollTimeModel(QObject *parent)
    : QAbstractListModel(parent)
{
    using namespace std::chrono_literals;

    m_times = {
        {i18nc("@item:inlistbox Poll expire times", "5 minutes"), std::chrono::seconds{5min}.count()},
        {i18nc("@item:inlistbox Poll expire times", "30 minutes"), std::chrono::seconds{30min}.count()},
        {i18nc("@item:inlistbox Poll expire times", "1 hour"), std::chrono::seconds{1h}.count()},
        {i18nc("@item:inlistbox Poll expire times", "6 hours"), std::chrono::seconds{6h}.count()},
        {i18nc("@item:inlistbox Poll expire times", "12 hours"), std::chrono::seconds{12h}.count()},
        {i18nc("@item:inlistbox Poll expire times", "1 day"), std::chrono::seconds{24h}.count()},
        {i18nc("@item:inlistbox Poll expire times", "3 days"), std::chrono::seconds{72h}.count()},
        {i18nc("@item:inlistbox Poll expire times", "7 days"), std::chrono::seconds{168h}.count()},
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
