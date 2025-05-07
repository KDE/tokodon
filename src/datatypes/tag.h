// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include "datatype.h"
#include <QQmlEngine>

class HistoryPrivate;

/// History associated to post
/// \see https://docs.joinmastodon.org/entities/History/
class History final
{
    TOKODON_GADGET(History, history)
    TOKODON_PROPERTY_READONLY(QDate, day)
    TOKODON_PROPERTY_READONLY(int, uses)
    TOKODON_PROPERTY_READONLY(int, accounts)
};

class TagPrivate;

/// Tag associated to post
/// \see https://docs.joinmastodon.org/entities/Tag/
class Tag final
{
    TOKODON_GADGET(Tag, tag)
    TOKODON_PROPERTY_READONLY(QString, name)
    TOKODON_PROPERTY_READONLY(QString, url)
    TOKODON_PROPERTY_READONLY(bool, following)
    TOKODON_PROPERTY_READONLY(QList<History>, history)
};
