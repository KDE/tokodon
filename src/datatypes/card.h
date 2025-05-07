// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include "datatype.h"

#include <QString>
#include <QJsonObject>

class PreviewCardAuthorPrivate;

/// Preview card authors associated to a Post
///
/// @see https://docs.joinmastodon.org/entities/PreviewCardAuthor
class PreviewCardAuthor final
{
    TOKODON_GADGET(PreviewCardAuthor, previewCardAuthor)

    TOKODON_PROPERTY_READONLY(QString, name)
    TOKODON_PROPERTY_READONLY(QString, description)
    TOKODON_PROPERTY_READONLY(QString, author)
};

class PreviewCardPrivate;

/// Preview card associated to a Post
///
/// @see https://docs.joinmastodon.org/entities/PreviewCard
class PreviewCard final
{
    TOKODON_GADGET(PreviewCard, previewCard)

    TOKODON_PROPERTY_READONLY(QUrl, url)
    TOKODON_PROPERTY_READONLY(QString, title)
    TOKODON_PROPERTY_READONLY(QString, description)
    TOKODON_PROPERTY_READONLY(QString, type)
    TOKODON_PROPERTY_READONLY(QList<PreviewCardAuthor>, authors)
    TOKODON_PROPERTY_READONLY(QString, providerName)
    TOKODON_PROPERTY_READONLY(QString, providerUrl)
    TOKODON_PROPERTY_READONLY(QString, html)
    TOKODON_PROPERTY_READONLY(int, width)
    TOKODON_PROPERTY_READONLY(int, height)
    TOKODON_PROPERTY_READONLY(QString, image)
    TOKODON_PROPERTY_READONLY(QString, embedUrl)
    TOKODON_PROPERTY_READONLY(QString, blurhash)
};
