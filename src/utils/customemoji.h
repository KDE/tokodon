// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonArray>

/**
 * @brief A custom emoji on a server, usually represented with colons e.g. :kde:
 */
class CustomEmoji
{
    Q_GADGET

    Q_PROPERTY(QString shortName MEMBER shortcode)
    Q_PROPERTY(QString unicode MEMBER url)
    Q_PROPERTY(bool isCustom MEMBER isCustom)

public:
    /**
     * @brief Parses an array of custom emoji, usually returned as a property with a key of "emojis".
     * @param json The JSON array to parse.
     */
    static QList<CustomEmoji> parseCustomEmojis(const QJsonArray &json);

    /**
     * @brief The shortcode name. For example, :kde: would have a shortcode of "kde"
     */
    QString shortcode;

    /**
     * @brief The static URL of the emoji.
     */
    QString url;

    /**
     * @brief Whether the emoji is custom, should always be true.
     */
    bool isCustom = true;
};

Q_DECLARE_METATYPE(CustomEmoji)
