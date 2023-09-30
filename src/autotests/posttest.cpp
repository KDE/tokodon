// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "mockaccount.h"
#include "timeline/poll.h"
#include "timeline/post.h"
#include <QJsonObject>

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
};

QTEST_MAIN(PostTest)
#include "posttest.moc"
