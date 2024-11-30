// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "utils/colorschemer.h"

#include <KColorSchemeManager>

ColorSchemer::ColorSchemer(QObject *parent)
    : QObject(parent)
    , c(KColorSchemeManager::instance())
{
}

ColorSchemer &ColorSchemer::instance()
{
    static ColorSchemer colorSchemer;
    return colorSchemer;
}

QAbstractItemModel *ColorSchemer::model() const
{
    return c->model();
}

void ColorSchemer::apply(int idx)
{
    c->activateScheme(c->model()->index(idx, 0));
}

void ColorSchemer::apply(const QString &name)
{
    c->activateScheme(c->indexForScheme(name));
}

int ColorSchemer::indexForScheme(const QString &name) const
{
    auto index = c->indexForScheme(name).row();
    if (index == -1) {
        index = 0;
    }
    return index;
}

QString ColorSchemer::nameForIndex(int index) const
{
    return c->model()->data(c->model()->index(index, 0), Qt::DisplayRole).toString();
}

#include "moc_colorschemer.cpp"
