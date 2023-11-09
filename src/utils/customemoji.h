// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QJsonArray>

/// Represents a custom emoji on a server, usually represents with colons e.g. :kde:
class CustomEmoji
{
    Q_GADGET

    Q_PROPERTY(QString shortName MEMBER shortcode)
    Q_PROPERTY(QString unicode MEMBER url)
    Q_PROPERTY(bool isCustom MEMBER isCustom)

public:
    /// Parses an array of custom emoji, usually returned as a property with a key of "emojis"
    /// \param json The JSON array to parse
    static QList<CustomEmoji> parseCustomEmojis(const QJsonArray &json);

    /// Replaces parts of a plaintext string that contain an existing custom emoji
    /// \param emojis The list of custom emojis, given from CustomEmoji::parseCustomEmojis()
    /// \param source The plaintext source to use
    /// \returns HTML to be used as rich text
    static QString replaceCustomEmojis(const QList<CustomEmoji> &emojis, const QString &source);

    /// The shortcode name. For example, :kde: would have a shortcode of "kde"
    QString shortcode;

    /// The static URL of the emoji
    QString url;

    /// Whether the emoji is custom, should always be true
    bool isCustom = true;
};

Q_DECLARE_METATYPE(CustomEmoji)
