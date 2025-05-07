// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "datatype.h"

class ApplicationPrivate;
class Application final
{
    TOKODON_GADGET(Application, application)

    TOKODON_PROPERTY_READONLY(QString, name)
    TOKODON_PROPERTY_READONLY(QString, website)
};
