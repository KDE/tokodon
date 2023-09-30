// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "utils/customemoji.h"

class CustomEmojiTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        QFile emojiFile(QLatin1String(DATA_DIR) + QLatin1Char('/') + QStringLiteral("emoji.json"));
        emojiFile.open(QFile::ReadOnly);

        doc = QJsonDocument::fromJson(emojiFile.readAll());
    }

    void testCustomEmojiParsing()
    {
        QVERIFY(doc.isArray());

        auto emojis = CustomEmoji::parseCustomEmojis(doc.array());
        QCOMPARE(emojis.size(), 2);
        QCOMPARE(emojis[0].shortcode, QStringLiteral("artaww"));
        QCOMPARE(emojis[0].url, QStringLiteral("https://cdn.masto.host/mastodonart/custom_emojis/images/000/181/127/static/63bd6a0097df7bbf.png"));
        QVERIFY(emojis[0].isCustom);
    }

    void testCustomEmojiReplacement()
    {
        auto emojis = CustomEmoji::parseCustomEmojis(doc.array());

        QString content{QStringLiteral(":artaww: :meowybara:")};

        content = CustomEmoji::replaceCustomEmojis(emojis, content);
        QCOMPARE(content,
                 QStringLiteral(
                     "<img height=\"16\" align=\"middle\" width=\"16\" "
                     "src=\"https://cdn.masto.host/mastodonart/custom_emojis/images/000/181/127/static/63bd6a0097df7bbf.png\"> <img height=\"16\" "
                     "align=\"middle\" width=\"16\" src=\"https://cdn.masto.host/mastodonart/custom_emojis/images/000/389/600/static/4dd38081c3f8f04c.png\">"));
    }

private:
    QJsonDocument doc;
};

QTEST_MAIN(CustomEmojiTest)
#include "customemojitest.moc"
