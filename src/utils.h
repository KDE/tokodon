// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "post.h"

QString visibilityToString(Post::Visibility visibility);

Post::Visibility stringToVisibility(const QString &visibility);
