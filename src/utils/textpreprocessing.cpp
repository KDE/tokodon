/**
 * SPDX-FileCopyrightText: 2023 Janet Black
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "textpreprocessing.h"

#include <QQuickTextDocument>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

TextPreprocessing::TextPreprocessing(QObject *parent)
    : QObject(parent)
{
}

TextPreprocessing::~TextPreprocessing()
{
}

static const auto fsi = QStringLiteral("\u2068");
static const auto pdi = QStringLiteral("\u2069");
static const auto lineSeparator = QStringLiteral("\u2028");

QString TextPreprocessing::preprocessHTML(const QString &html, const QColor &linkColor)
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

#include "moc_textpreprocessing.cpp"
