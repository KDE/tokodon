// SPDX-FileCopyrightText: 2023 Janet Black
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QtQml>

#include "utils/customemoji.h"

/**
 * @brief Handles some miscellaneous text processing tasks.
 */
class TextHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    /**
     * @brief Fixes bidirectionality in posts.
     *
     * Ensures Right-to-Left languages such as Arabic stay that way, and any Left-to-Right languages such as English are separated. See BUG 475043 for more
     * details on why this is necessary.
     *
     * @param html The HTML text to process.
     * @param font The font to use.
     * @return The processed HTML, do note that we use QTextDocument so the HTML may be littered with unnecessary styling and other garbage.
     */
    static Q_INVOKABLE QString fixBidirectionality(const QString &html, const QFont &font);

    /**
     * @brief Parses a HTML body and returns a processed body and a list of tags respectively.
     * @param contentHtml The HTML to process.
     * @return The processed HTML as the first item in the pair, and the list of standalone tags (if any) as the second item.
     */
    static QPair<QString, QList<QString>> removeStandaloneTags(QString contentHtml);

    /**
     * @brief Replaces parts of a plaintext string that contain an existing custom emoji.
     * @param emojis The list of custom emojis, given from CustomEmoji::parseCustomEmojis()
     * @param source The plaintext source to use.
     * @return HTML to be used as rich text.
     */
    static QString replaceCustomEmojis(const QList<CustomEmoji> &emojis, const QString &source);

    /**
     * @brief Determines whether or not a URL could possibly be a post.
     * @note This isn't supposed to be perfect, but catch the 99% case.
     */
    static bool isPostUrl(const QString &url);
};

namespace TextRegex
{
static const QRegularExpression
    url(QStringLiteral(R"(\b((www\.(?!\.)(?!(\w|\.|-)+@)|https?:(//)?\w)(&(?![lg]t;)|[^&\s<>'"])+(&(?![lg]t;)|[^&!,.\s<>'"\]):])))"),
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption);
static const QRegularExpression hashtagExp(QStringLiteral("(?:<a\\b[^>]*>#<span>(\\S*)<\\/span><\\/a>)"));
static const QRegularExpression extraneousParagraphExp(QStringLiteral("(\\s*(?:<(?:p|br)\\s*\\/?>)+\\s*<\\/p>)"));
static const QRegularExpression extraneousBreakExp(QStringLiteral("(\\s*(?:<br\\s*\\/?>)+\\s*)<\\/p>"));
}
