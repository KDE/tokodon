// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QtTest/QtTest>

#include "timeline/post.h"
#include <QAbstractItemModelTester>
#include <QSignalSpy>

class ContentParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
    }

    void testTagExtraction()
    {
        QString testHtml = QStringLiteral(
            "<p>Heya, just letting you know that I offer a membership on my cofi<br>There I share early wips, short commentary about my work, and rare "
            "sketches.<br>Do join and support me in more food/mushrooms/etc pixel art!</p><p><a href=\"http://ko-fi.com/maruki/tiers\" rel=\"nofollow noopener "
            "noreferrer\" translate=\"no\" target=\"_blank\"><span class=\"invisible\">http://</span><span class=\"\">ko-fi.com/maruki/tiers</span><span "
            "class=\"invisible\"></span></a></p><p><a href=\"https://mastodon.social/tags/art\" class=\"mention hashtag\" rel=\"nofollow noopener noreferrer\" "
            "target=\"_blank\">#<span>art</span></a> <a href=\"https://mastodon.social/tags/creativetoots\" class=\"mention hashtag\" rel=\"nofollow noopener "
            "noreferrer\" target=\"_blank\">#<span>creativetoots</span></a> <a href=\"https://mastodon.social/tags/pixelart\" class=\"mention hashtag\" "
            "rel=\"nofollow noopener noreferrer\" target=\"_blank\">#<span>pixelart</span></a></p>");

        auto parsed = Post::parseContent(testHtml);
        QCOMPARE(parsed.first, testHtml);
    }
};

QTEST_MAIN(ContentParserTest)
#include "contentparsertest.moc"
