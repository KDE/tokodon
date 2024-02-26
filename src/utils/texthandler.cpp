/**
 * SPDX-FileCopyrightText: 2023 Janet Black
 * SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "texthandler.h"
#include "customemoji.h"

#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

using namespace Qt::StringLiterals;

TextHandler::TextHandler(QObject *parent)
    : QObject(parent)
{
}

TextHandler::~TextHandler() = default;

static const auto fsi = QStringLiteral("\u2068");
static const auto pdi = QStringLiteral("\u2069");
static const auto lineSeparator = QStringLiteral("\u2028");

QString TextHandler::preprocessHTML(const QString &html, const QColor &linkColor)
{
    QTextDocument doc;
    doc.setHtml(html);

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
                auto format = it.charFormat();
                format.setForeground(linkColor);
                format.setFontUnderline(false);
                format.setUnderlineStyle(QTextCharFormat::NoUnderline);

                QTextCursor curs(&doc);
                curs.setPosition(it.position(), QTextCursor::MoveAnchor);
                curs.setPosition(it.position() + it.length(), QTextCursor::KeepAnchor);
                curs.setCharFormat(format);
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

QString TextHandler::computeContent(const QJsonObject &obj, std::shared_ptr<Identity> authorIdentity)
{
    const auto emojis = CustomEmoji::parseCustomEmojis(obj["emojis"_L1].toArray());
    QString content = CustomEmoji::replaceCustomEmojis(emojis, obj["content"_L1].toString());

    const auto tags = obj["tags"_L1].toArray();
    const QString baseUrl = authorIdentity->url().toDisplayString(QUrl::RemovePath);

    for (const auto &tag : tags) {
        const auto tagObj = tag.toObject();

        const QList<QString> tagFormats = {
            QStringLiteral("tags"), // Mastodon
            QStringLiteral("tag") // Akkoma/Pleroma
        };

        for (const QString &tagFormat : tagFormats) {
            content = content.replace(baseUrl + QStringLiteral("/%1/").arg(tagFormat) + tagObj["name"_L1].toString(),
                                      QStringLiteral("hashtag:/") + tagObj["name"_L1].toString(),
                                      Qt::CaseInsensitive);
        }
    }

    const auto mentions = obj["mentions"_L1].toArray();

    for (const auto &mention : mentions) {
        const auto mentionObj = mention.toObject();
        content = content.replace(mentionObj["url"_L1].toString(), QStringLiteral("account:/") + mentionObj["id"_L1].toString(), Qt::CaseInsensitive);
    }

    return content;
}

QPair<QString, QList<QString>> TextHandler::parseContent(const QString &html)
{
    const QRegularExpression hashtagExp(QStringLiteral("(?:<a\\b[^>]*>#<span>(\\S*)<\\/span><\\/a>)"));
    const QRegularExpression extraneousParagraph(QStringLiteral("(\\s*(?:<(?:p|br)\\s*\\/?>)+\\s*<\\/p>)"));

    QList<QString> standaloneTags;

    // Find the last <p> or <br>
    const int lastBreak = html.lastIndexOf(QStringLiteral("<br>"));
    int lastParagraphBegin = html.lastIndexOf(QStringLiteral("<p>"));
    if (lastBreak > lastParagraphBegin) {
        lastParagraphBegin = lastBreak;
    }

    const int lastParagraphEnd = html.lastIndexOf(QStringLiteral("</p>"));
    QString lastParagraph = html.mid(lastParagraphBegin, lastParagraphEnd - html.length());

    QString processedHtml = html;

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
        auto extraneousIterator = extraneousParagraph.globalMatch(possibleLastParagraph);
        if (extraneousIterator.hasNext()) {
            processedHtml.replace(lastParagraph, possibleLastParagraph);
            standaloneTags = possibleTags;
        }
    }

    const QRegularExpression extraneousBreakExp(QStringLiteral("(\\s*(?:<br\\s*\\/?>)+\\s*)<\\/p>"));

    // Ensure we remove any remaining <br>'s which will mess up the spacing in a post.
    // Example: "<p>Yosemite Valley reflections with rock<br />    </p>"
    {
        auto matchIterator = extraneousBreakExp.globalMatch(processedHtml);
        while (matchIterator.hasNext()) {
            const QRegularExpressionMatch match = matchIterator.next();
            processedHtml = processedHtml.replace(match.captured(1), QStringLiteral(""));
        }
    }

    // Ensure we remove any empty <p>'s which will mess up the spacing in a post.
    // Example: "<p>Boris Karloff (again) as Imhotep</p><p>  </p>"
    {
        auto matchIterator = extraneousParagraph.globalMatch(processedHtml);
        while (matchIterator.hasNext()) {
            const QRegularExpressionMatch match = matchIterator.next();
            processedHtml = processedHtml.replace(match.captured(1), QStringLiteral(""));
        }
    }

    return {processedHtml, standaloneTags};
}

#include "moc_texthandler.cpp"
