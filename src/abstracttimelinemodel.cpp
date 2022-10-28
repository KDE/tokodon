// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstracttimelinemodel.h"

AbstractTimelineModel::AbstractTimelineModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

bool AbstractTimelineModel::loading() const
{
    return m_loading;
}
