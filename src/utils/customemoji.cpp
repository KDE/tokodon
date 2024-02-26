// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils/customemoji.h"

QList<CustomEmoji> CustomEmoji::parseCustomEmojis(const QJsonArray &json)
{
    QList<CustomEmoji> emojis;
    for (auto emojiObj : json) {
        if (!emojiObj.isObject()) {
            continue;
        }

        CustomEmoji customEmoji{};
        customEmoji.shortcode = emojiObj[QStringLiteral("shortcode")].toString();
        customEmoji.url = emojiObj[QStringLiteral("static_url")].toString();

        emojis.push_back(customEmoji);
    }

    return emojis;
}

#include "moc_customemoji.cpp"