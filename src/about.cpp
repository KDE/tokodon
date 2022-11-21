// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>

#include "about.h"

KAboutData AboutType::aboutData() const
{
    return KAboutData::applicationData();
}
