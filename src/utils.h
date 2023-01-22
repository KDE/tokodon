// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "post.h"

QString visibilityToString(Post::Visibility visibility);

Post::Visibility stringToVisibility(const QString &visibility);

template<typename T>
class asKeyValueRange
{
public:
    asKeyValueRange(T &data)
        : m_data{data}
    {
    }

    auto begin()
    {
        return m_data.keyValueBegin();
    }

    auto end()
    {
        return m_data.keyValueEnd();
    }

private:
    T &m_data;
};
