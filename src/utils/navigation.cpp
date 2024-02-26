// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "utils/navigation.h"

Navigation &Navigation::instance()
{
    static Navigation navigation;
    return navigation;
}

#include "moc_navigation.cpp"