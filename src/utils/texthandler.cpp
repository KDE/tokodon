// SPDX-FileCopyrightText: 2023 Janet Black
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "utils/texthandler.h"

#include <KLocalizedString>
#include <QQuickTextDocument>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

using namespace Qt::StringLiterals;

static const auto fsi = QStringLiteral("\u2068");
static const auto pdi = QStringLiteral("\u2069");
static const auto lineSeparator = QStringLiteral("\u2028");

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
        for (auto fragment = block.begin(); fragment != block.end(); ++fragment) {
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

    // Catch all the tags in the last paragraph of the post, but only if they are not surrounded by text
    {
        const qsizetype lastParagraphEnd = contentHtml.lastIndexOf(QStringLiteral("</p>"));
        const QString lastParagraph = contentHtml.mid(lastParagraphBegin, lastParagraphEnd - contentHtml.length());
        QList<QString> possibleTags;
        QString possibleLastParagraph = lastParagraph;

        auto matchIterator = TextRegex::hashtagExp.globalMatch(possibleLastParagraph);
        while (matchIterator.hasNext()) {
            const QRegularExpressionMatch match = matchIterator.next();
            possibleTags.push_back(match.captured(1));
            possibleLastParagraph = possibleLastParagraph.replace(match.captured(0), QStringLiteral(""));
        }

        // If this paragraph is truly extraneous, then we can take its tags, otherwise skip.
        const auto extraneousIterator = TextRegex::extraneousParagraphExp.globalMatch(possibleLastParagraph);
        if (extraneousIterator.hasNext()) {
            contentHtml.replace(lastParagraph, possibleLastParagraph);
            standaloneTags = possibleTags;
        }
    }

    // Ensure we remove any remaining <br>'s which will mess up the spacing in a post.
    // Example: "<p>Yosemite Valley reflections with rock<br />    </p>"
    {
        auto matchIterator = TextRegex::extraneousBreakExp.globalMatch(contentHtml);
        while (matchIterator.hasNext()) {
            const QRegularExpressionMatch match = matchIterator.next();
            contentHtml = contentHtml.replace(match.captured(1), QStringLiteral(""));
        }
    }

    // Ensure we remove any empty <p>'s which will mess up the spacing in a post.
    // Example: "<p>Boris Karloff (again) as Imhotep</p><p>  </p>"
    {
        auto matchIterator = TextRegex::extraneousParagraphExp.globalMatch(contentHtml);
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

bool TextHandler::isPostUrl(const QString &url)
{
    // Check if the URL is behind HTTPs, that's a good indicator as any.
    if (!url.contains(QStringLiteral("https"))) {
        return false;
    }

    // Then check if the URL is especially Mastodon-like.
    static QRegularExpression mastodonRegex(QStringLiteral("(/@\\w*)/+(\\d*)"));
    const auto match = mastodonRegex.match(url);

    // There are 3 capture groups, first is the global. Second is the username, third is the post id.
    if (match.hasMatch() && match.capturedTexts().length() == 3) {
        return true;
    }

    // TODO: create regex to better whittle down these cases:
    // Pleroma/Akkoma/Misskey
    if (url.contains(QStringLiteral("/notes/"))) {
        return true;
    }

    // Pixelfed
    if (url.contains(QStringLiteral("/p/"))) {
        return true;
    }

    return false;
}

void TextHandler::forceRefreshTextDocument(QQuickTextDocument *textDocument, QQuickItem *item)
{
    // HACK: Workaround bug QTBUG 93281, only applies to <6.7
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    connect(textDocument->textDocument(), SIGNAL(imagesLoaded()), item, SLOT(updateWholeDocument()));
#else
    Q_UNUSED(textDocument)
    Q_UNUSED(item)
#endif
}

QString TextHandler::getRelativeDateTime(const QDateTime &dateTime)
{
    const auto current = QDateTime::currentDateTime();
    const auto secsTo = dateTime.secsTo(current);
    if (secsTo < 0) {
        return i18n("in the future");
    } else if (secsTo < 60) {
        return i18n("%1s", qCeil(secsTo));
    } else if (secsTo < 60 * 60) {
        return i18n("%1m", qCeil(secsTo / 60));
    } else if (secsTo < 60 * 60 * 24) {
        return i18n("%1h", qCeil(secsTo / (60 * 60)));
    } else {
        return getRelativeDate(dateTime.date());
    }
}

QString TextHandler::getRelativeDate(const QDate &dateTime)
{
    const auto current = QDate::currentDate();
    const auto daysTo = dateTime.daysTo(current);
    if (daysTo == 0) {
        return i18n("Today");
    } else if (daysTo < 7) {
        return i18n("%1d", qCeil(daysTo));
    } else if (daysTo < 365) {
        const auto weeksTo = qCeil(daysTo / 7);
        if (weeksTo < 5) {
            return i18np("1 week ago", "%1 weeks ago", weeksTo);
        } else {
            const auto monthsTo = qCeil(daysTo / 30);
            return i18np("1 month ago", "%1 months ago", monthsTo);
        }
    } else {
        const auto yearsTo = qCeil(daysTo / 365);
        return i18np("1 year ago", "%1 years ago", yearsTo);
    }
}

#include "moc_texthandler.cpp"