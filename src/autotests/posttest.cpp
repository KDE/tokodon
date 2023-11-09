// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "mockaccount.h"

using namespace Qt::Literals::StringLiterals;

class PostTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
    }

    void testFromJson()
    {
        MockAccount account;

        QFile statusExampleApi;
        statusExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + "status.json"_L1);
        statusExampleApi.open(QIODevice::ReadOnly);

        const auto doc = QJsonDocument::fromJson(statusExampleApi.readAll());
        Post post(&account, doc.object());

        QCOMPARE(post.spoilerText(), QStringLiteral("SPOILER"));
        QCOMPARE(post.content(), QStringLiteral("<p>LOREM</p>"));
        QVERIFY(post.card());
        QCOMPARE(post.contentType(), QString());
        QCOMPARE(post.sensitive(), false);
        QCOMPARE(post.visibility(), Post::Visibility::Public);
        QCOMPARE(post.wasEdited(), false);

        QCOMPARE(post.authorIdentity()->displayName(), QStringLiteral("Eugen :kde:"));
        QCOMPARE(post.authorIdentity()->displayNameHtml(), QStringLiteral("Eugen <img height=\"16\" align=\"middle\" width=\"16\" src=\"https://kde.org\">"));
    }

    void testFromJsonWithPoll()
    {
        MockAccount account;

        QFile statusExampleApi;
        statusExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + "status-poll.json"_L1);
        statusExampleApi.open(QIODevice::ReadOnly);

        const auto doc = QJsonDocument::fromJson(statusExampleApi.readAll());
        Post post(&account, doc.object());

        QCOMPARE(post.wasEdited(), true);
        QVERIFY(post.poll());
        const auto poll = post.poll();
        QCOMPARE(poll->id(), QStringLiteral("34830"));
        QCOMPARE(poll->expiresAt().date().year(), 2019);
        QCOMPARE(poll->expired(), true);
        QCOMPARE(poll->multiple(), false);
        QCOMPARE(poll->votesCount(), 10);
        QCOMPARE(poll->votersCount(), -1);
        QCOMPARE(poll->voted(), true);
        const auto ownVotes = poll->ownVotes();
        QCOMPARE(ownVotes.count(), 1);
        QCOMPARE(ownVotes[0], 1);
        const auto options = poll->options();
        QCOMPARE(options.count(), 2);
        QCOMPARE(options[0]["title"_L1], QStringLiteral("accept"));
        QCOMPARE(options[0]["votesCount"_L1], 6);
        QCOMPARE(options[1]["title"_L1], QStringLiteral("deny <img height=\"16\" align=\"middle\" width=\"16\" src=\"https://kde.org\">"));
        QCOMPARE(options[1]["votesCount"_L1], 4);
    }

    // Normal case
    void testContentParsing()
    {
        MockAccount account;

        QFile statusExampleApi;
        statusExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + "status-tags.json"_L1);
        statusExampleApi.open(QIODevice::ReadOnly);

        const auto doc = QJsonDocument::fromJson(statusExampleApi.readAll());
        Post post(&account, doc.object());

        const QVector<QString> standaloneTags = {QStringLiteral("blackandwhite"),
                                                 QStringLiteral("photo"),
                                                 QStringLiteral("monochrome"),
                                                 QStringLiteral("landscape"),
                                                 QStringLiteral("photography")};

        QCOMPARE(post.content(), QStringLiteral("<p>Yosemite Valley reflections with rock</p>"));
        QCOMPARE(post.standaloneTags(), standaloneTags);
    }

    // Ensure that extra <p>'s are removed
    void testContentParsingEdgeCaseOne()
    {
        const QString testHtml = QStringLiteral(
            "<p>Boris Karloff (again) as Imhotep</p><p><a href=\"https://mastodon.art/tags/Inktober\" class=\"mention hashtag\" "
            "rel=\"tag\">#<span>Inktober</span></a> <a href=\"https://mastodon.art/tags/Halloween\" class=\"mention hashtag\" "
            "rel=\"tag\">#<span>Halloween</span></a> <a href=\"https://mastodon.art/tags/TheMummy\" class=\"mention hashtag\" "
            "rel=\"tag\">#<span>TheMummy</span></a></p>");

        const auto [content, tags] = Post::parseContent(testHtml);

        QCOMPARE(content, QStringLiteral("<p>Boris Karloff (again) as Imhotep</p>"));
    }

    // Ensure that unicode/non-english characters (like Japanese) are picked up by the parser
    void testContentParsingEdgeCaseTwo()
    {
        const QString testHtml = QStringLiteral(
            R"(<p>cropping of homura and madoka <br />\uD83C\uDF80\uD83E\uDDA2\uD83C\uDF38\uD83C\uDFF9✨</p><p>finished version here - <a href=\"https://floodkiss.tumblr.com/post/682418812978659328/even-if-you-cant-see-me-even-if-you-cant-hear\" target=\"_blank\" rel=\"nofollow noopener noreferrer\" translate=\"no\"><span class=\"invisible\">https://</span><span class=\"ellipsis\">floodkiss.tumblr.com/post/6824</span><span class=\"invisible\">18812978659328/even-if-you-cant-see-me-even-if-you-cant-hear</span></a></p><p><a href=\"https://mastodon.art/tags/MadokaMagica\" class=\"mention hashtag\" rel=\"tag\">#<span>MadokaMagica</span></a> <a href=\"https://mastodon.art/tags/%E9%AD%94%E6%B3%95%E5%B0%91%E5%A5%B3%E3%81%BE%E3%81%A9%E3%81%8B%E3%83%9E%E3%82%AE%E3%82%AB\" class=\"mention hashtag\" rel=\"tag\">#<span>魔法少女まどかマギカ</span></a> <a href=\"https://mastodon.art/tags/FediArt\" class=\"mention hashtag\" rel=\"tag\">#<span>FediArt</span></a> <a href=\"https://mastodon.art/tags/MastoArt\" class=\"mention hashtag\" rel=\"tag\">#<span>MastoArt</span></a> <a href=\"https://mastodon.art/tags/FanArt\" class=\"mention hashtag\" rel=\"tag\">#<span>FanArt</span></a> <a href=\"https://mastodon.art/tags/HomuraAkemi\" class=\"mention hashtag\" rel=\"tag\">#<span>HomuraAkemi</span></a> <a href=\"https://mastodon.art/tags/MadokaKaname\" class=\"mention hashtag\" rel=\"tag\">#<span>MadokaKaname</span></a></p>)");

        const auto [content, tags] = Post::parseContent(testHtml);

        const QString expected = QStringLiteral(
            R"(<p>cropping of homura and madoka <br />\uD83C\uDF80\uD83E\uDDA2\uD83C\uDF38\uD83C\uDFF9✨</p><p>finished version here - <a href=\"https://floodkiss.tumblr.com/post/682418812978659328/even-if-you-cant-see-me-even-if-you-cant-hear\" target=\"_blank\" rel=\"nofollow noopener noreferrer\" translate=\"no\"><span class=\"invisible\">https://</span><span class=\"ellipsis\">floodkiss.tumblr.com/post/6824</span><span class=\"invisible\">18812978659328/even-if-you-cant-see-me-even-if-you-cant-hear</span></a></p>)");

        QCOMPARE(tags[1], QStringLiteral("魔法少女まどかマギカ"));
        QCOMPARE(content, expected);
    }
};

QTEST_MAIN(PostTest)
#include "posttest.moc"
