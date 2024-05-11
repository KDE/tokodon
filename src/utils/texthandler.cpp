// SPDX-FileCopyrightText: 2023 Janet Black
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "utils/texthandler.h"

#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

using namespace Qt::StringLiterals;

static const auto fsi = QStringLiteral("\u2068");
static const auto pdi = QStringLiteral("\u2069");
static const auto lineSeparator = QStringLiteral("\u2028");

static const QRegularExpression hashtagExp(QStringLiteral("(?:<a\\b[^>]*>#<span>(\\S*)<\\/span><\\/a>)"));
static const QRegularExpression extraneousParagraphExp(QStringLiteral("(\\s*(?:<(?:p|br)\\s*\\/?>)+\\s*<\\/p>)"));
static const QRegularExpression extraneousBreakExp(QStringLiteral("(\\s*(?:<br\\s*\\/?>)+\\s*)<\\/p>"));

QString TextHandler::fixBidirectionality(const QString &html, const QFont &font)
{
    QTextDocument doc;
    doc.setHtml(html);
    doc.setDefaultFont(font);

    // transform mentions into isolates
    // this causes mentions to effectively be treated as opaque and non-text for
    // the purposes of the bidirectionality algorithim, which is how users expect
    // them to behave
    // pt 1 of 475043 fix
    for (auto block = doc.begin(); block != doc.end(); block = block.next()) {
        for (auto fragment = block.begin(); fragment != block.end(); fragment++) {
            auto it = fragment.fragment();

            if (it.charFormat().isAnchor() && it.text().startsWith(QStringLiteral("@"))) {
                QTextCursor curs(&doc);
                curs.setPosition(it.position(), QTextCursor::MoveAnchor);
                curs.setPosition(it.position() + it.length(), QTextCursor::KeepAnchor);
                curs.insertText(fsi + it.text() + pdi);
            }

            // while we're iterating through fragments we might as well set the kirigami
            // link colour here
            if (it.charFormat().isAnchor()) {
                QTextCursor curs(&doc);
                curs.setPosition(it.position(), QTextCursor::MoveAnchor);
                curs.setPosition(it.position() + it.length(), QTextCursor::KeepAnchor);
            }
        }
    }

    // split apart <br> into different text blocks
    // so that different spans can have different layout directions
    // better matching bidirectionality behaviour that users are expecting
    // pt 2 of 475043 fix
    for (auto cursor = doc.find(lineSeparator); !cursor.isNull(); cursor = doc.find(lineSeparator, cursor)) {
        cursor.insertBlock();
    }

    return doc.toHtml();
}

QPair<QString, QList<QString>> TextHandler::removeStandaloneTags(QString contentHtml)
{
    // Finally, find the "standalone tags" for the post, so we can display them separately.
    // These usually appear in the last paragraph or so. We also do some extra processing to ensure there aren't blank lines left over.
    QList<QString> standaloneTags;

    // Find the last <p> or <br>
    const qsizetype lastBreak = contentHtml.lastIndexOf(QStringLiteral("<br>"));
    qsizetype lastParagraphBegin = contentHtml.lastIndexOf(QStringLiteral("<p>"));
    if (lastBreak > lastParagraphBegin) {
        lastParagraphBegin = lastBreak;
    }

    const qsizetype lastParagraphEnd = contentHtml.lastIndexOf(QStringLiteral("</p>"));
    QString lastParagraph = contentHtml.mid(lastParagraphBegin, lastParagraphEnd - contentHtml.length());

    // Catch all the tags in the last paragraph of the post, but only if they are not surrounded by text
    {
        QList<QString> possibleTags;
        QString possibleLastParagraph = lastParagraph;

        auto matchIterator = hashtagExp.globalMatch(possibleLastParagraph);
        while (matchIterator.hasNext()) {
            const QRegularExpressionMatch match = matchIterator.next();
            possibleTags.push_back(match.captured(1));
            possibleLastParagraph = possibleLastParagraph.replace(match.captured(0), QStringLiteral(""));
        }

        // If this paragraph is truly extraneous, then we can take its tags, otherwise skip.
        auto extraneousIterator = extraneousParagraphExp.globalMatch(possibleLastParagraph);
        if (extraneousIterator.hasNext()) {
            contentHtml.replace(lastParagraph, possibleLastParagraph);
            standaloneTags = possibleTags;
        }
    }

    // Ensure we remove any remaining <br>'s which will mess up the spacing in a post.
    // Example: "<p>Yosemite Valley reflections with rock<br />    </p>"
    {
        auto matchIterator = extraneousBreakExp.globalMatch(contentHtml);
        while (matchIterator.hasNext()) {
            const QRegularExpressionMatch match = matchIterator.next();
            contentHtml = contentHtml.replace(match.captured(1), QStringLiteral(""));
        }
    }

    // Ensure we remove any empty <p>'s which will mess up the spacing in a post.
    // Example: "<p>Boris Karloff (again) as Imhotep</p><p>  </p>"
    {
        auto matchIterator = extraneousParagraphExp.globalMatch(contentHtml);
        while (matchIterator.hasNext()) {
            const QRegularExpressionMatch match = matchIterator.next();
            contentHtml = contentHtml.replace(match.captured(1), QStringLiteral(""));
        }
    }

    return {contentHtml, standaloneTags};
}

QString TextHandler::replaceCustomEmojis(const QList<CustomEmoji> &emojis, const QString &source)
{
    QString processed = source;
    for (const auto &emoji : emojis) {
        processed = processed.replace(QLatin1Char(':') + emoji.shortcode + QLatin1Char(':'),
                                      QStringLiteral("<img height=\"16\" align=\"middle\" width=\"16\" src=\"") + emoji.url + QStringLiteral("\">"));
    }

    return processed;
}

#include "moc_texthandler.cpp"