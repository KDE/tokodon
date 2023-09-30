// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "utils.h"

static QMap<Post::Visibility, QString> p_visibilityToString = {
    {Post::Visibility::Public, QStringLiteral("public")},
    {Post::Visibility::Unlisted, QStringLiteral("unlisted")},
    {Post::Visibility::Private, QStringLiteral("private")},
    {Post::Visibility::Direct, QStringLiteral("direct")},
};

static QMap<QString, Post::Visibility> p_stringToVisibility = {
    {QStringLiteral("public"), Post::Visibility::Public},
    {QStringLiteral("unlisted"), Post::Visibility::Unlisted},
    {QStringLiteral("private"), Post::Visibility::Private},
    {QStringLiteral("direct"), Post::Visibility::Direct},
    {QStringLiteral("local"), Post::Visibility::Local},
};

QString visibilityToString(Post::Visibility visibility)
{
    return p_visibilityToString[visibility];
}

Post::Visibility stringToVisibility(const QString &visibility)
{
    return p_stringToVisibility[visibility];
}
