// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "customemoji.h"

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

QString CustomEmoji::replaceCustomEmojis(const QList<CustomEmoji> &emojis, const QString &source)
{
    QString processed = source;
    for (const auto &emoji : emojis) {
        processed = processed.replace(QLatin1Char(':') + emoji.shortcode + QLatin1Char(':'),
                                      QStringLiteral("<img height=\"16\" align=\"middle\" width=\"16\" src=\"") + emoji.url + QStringLiteral("\">"));
    }

    return processed;
}
