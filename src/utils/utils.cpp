// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "utils.h"

static QMap<Post::Visibility, QString> p_visibilityToString = {
    {Post::Visibility::Public, "public"},
    {Post::Visibility::Unlisted, "unlisted"},
    {Post::Visibility::Private, "private"},
    {Post::Visibility::Direct, "direct"},
};

static QMap<QString, Post::Visibility> p_stringToVisibility = {
    {"public", Post::Visibility::Public},
    {"unlisted", Post::Visibility::Unlisted},
    {"private", Post::Visibility::Private},
    {"direct", Post::Visibility::Direct},
    {"local", Post::Visibility::Local},
};

QString visibilityToString(Post::Visibility visibility)
{
    return p_visibilityToString[visibility];
}

Post::Visibility stringToVisibility(const QString &visibility)
{
    return p_stringToVisibility[visibility];
}
