// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "mockaccount.h"
#include "post.h"
#include "poll.h"
#include <QJsonObject>
#include <qstringliteral.h>

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
        statusExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + "status.json");
        statusExampleApi.open(QIODevice::ReadOnly);

        const auto doc = QJsonDocument::fromJson(statusExampleApi.readAll());
        Post post(&account, doc.object());

        QCOMPARE(post.subject(), "SPOILER");
        QCOMPARE(post.content(), "<p>LOREM</p>");
        QVERIFY(post.card());
        QCOMPARE(post.contentType(), QString());
        QCOMPARE(post.isSensitive(), false);
        QCOMPARE(post.visibility(), Post::Visibility::Public);

        QCOMPARE(post.authorIdentity()->displayName(), "Eugen :kde:");
        QCOMPARE(post.authorIdentity()->displayNameHtml(), "Eugen <img height=\"16\" align=\"middle\" width=\"16\" src=\"https://kde.org\">");
    }

    void testFromJsonWithPoll()
    {
        MockAccount account;

        QFile statusExampleApi;
        statusExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + "status-poll.json");
        statusExampleApi.open(QIODevice::ReadOnly);

        const auto doc = QJsonDocument::fromJson(statusExampleApi.readAll());
        Post post(&account, doc.object());

        QVERIFY(post.poll());
        const auto poll = post.poll();
        QCOMPARE(poll->id(), "34830");
        QCOMPARE(poll->expiresAt().date().year(), 2019);
        QCOMPARE(poll->expired(), true);
        QCOMPARE(poll->multiple(), false);
        QCOMPARE(poll->votesCount(), 10);
        QCOMPARE(poll->votersCount(), -1);
        QCOMPARE(poll->voted(), true);
        QCOMPARE(poll->ownVotes().count(), 1);
        QCOMPARE(poll->ownVotes()[0], 1);
        QCOMPARE(poll->options().count(), 2);
        QCOMPARE(poll->options()[0].title(), QStringLiteral("accept"));
        QCOMPARE(poll->options()[0].votesCount(), 6);
        QCOMPARE(poll->options()[1].title(), QStringLiteral("deny <img height=\"16\" align=\"middle\" width=\"16\" src=\"https://kde.org\">"));
        QCOMPARE(poll->options()[1].votesCount(), 4);
    }
};

QTEST_MAIN(PostTest)
#include "posttest.moc"
