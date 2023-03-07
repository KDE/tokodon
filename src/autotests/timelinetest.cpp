// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "helperreply.h"
#include "maintimelinemodel.h"
#include "mockaccount.h"
#include "poll.h"
#include "post.h"
#include "tagsmodel.h"
#include "threadmodel.h"
#include <KLocalizedString>
#include <QAbstractItemModelTester>
#include <QSignalSpy>

class TimelineTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        AccountManager::instance().disableSettings(true);

        account = new MockAccount();
        AccountManager::instance().addAccount(account);
        AccountManager::instance().selectAccount(account);
    }

    void cleanupTestCase()
    {
        AccountManager::instance().removeAccount(account);
    }

    void testMainDisplayName()
    {
        KLocalizedString::setApplicationDomain("tokodon");
        KLocalizedString::setLanguages(QStringList{"C"});
        account->setUsername("test");

        MainTimelineModel timelineModel;
        timelineModel.setName("public");
        QCOMPARE(timelineModel.displayName(), "Local Timeline");
        timelineModel.setName("federated");
        QCOMPARE(timelineModel.displayName(), "Global Timeline");
        timelineModel.setName("home");
        QCOMPARE(timelineModel.displayName(), "Home");

        auto account2 = new MockAccount();
        AccountManager::instance().addAccount(account2);
        QCOMPARE(timelineModel.displayName(), "Home (test)");
    }

    void testStreamUpdate()
    {
        QFile statusExampleApi;
        statusExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + "status.json");
        statusExampleApi.open(QIODevice::ReadOnly);

        MainTimelineModel timelineModel;
        QCOMPARE(timelineModel.rowCount({}), 0);

        account->streamingEvent(AbstractAccount::StreamingEventType::UpdateEvent, statusExampleApi.readAll());
        QCOMPARE(timelineModel.rowCount({}), 1);
    }

    void testFillTimelineMain()
    {
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/timelines/home")), new TestReply("statuses.json", account));
        auto fetchMoreUrl = account->apiUrl(QStringLiteral("/api/v1/timelines/home"));
        fetchMoreUrl.setQuery(QUrlQuery{
            {"max_id", "103270115826038975"},
        });
        account->registerGet(fetchMoreUrl, new TestReply("statuses.json", account));

        MainTimelineModel timelineModel;

        QCOMPARE(timelineModel.rowCount({}), 2);
        QVERIFY(timelineModel.canFetchMore({}));
        timelineModel.fetchMore({});
        QCOMPARE(timelineModel.rowCount({}), 4);
    }

    void testTagModel()
    {
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/timelines/tag/home")), new TestReply("statuses.json", account));
        auto fetchMoreUrl = account->apiUrl(QStringLiteral("/api/v1/timelines/tag/home"));
        fetchMoreUrl.setQuery(QUrlQuery{
            {"max_id", "103270115826038975"},
        });
        account->registerGet(fetchMoreUrl, new TestReply("statuses.json", account));

        TagsModel tagModel;
        tagModel.setHashtag("home");

        QCOMPARE(tagModel.rowCount({}), 2);
        QVERIFY(tagModel.canFetchMore({}));
        tagModel.fetchMore({});
        QCOMPARE(tagModel.rowCount({}), 4);
    }

    void testThreadModel()
    {
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/statuses/103270115826048975")), new TestReply("status.json", account));
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/statuses/103270115826048975/context")), new TestReply("context.json", account));

        ThreadModel threadModel;
        threadModel.setPostId(QStringLiteral("103270115826048975"));
        QCOMPARE(threadModel.rowCount({}), 3);
        QCOMPARE(threadModel.data(threadModel.index(1, 0), AbstractTimelineModel::SelectedRole).toBool(), true);
        QCOMPARE(threadModel.displayName(), "Thread");
        QCOMPARE(threadModel.postId(), "103270115826048975");
        QCOMPARE(threadModel.canFetchMore({}), false);
    }

    void testModelPoll()
    {
        MainTimelineModel timelineModel;
        timelineModel.setName("home");

        QFile statusExampleApi;
        statusExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + "status-poll.json");
        statusExampleApi.open(QIODevice::ReadOnly);
        account->streamingEvent(AbstractAccount::StreamingEventType::UpdateEvent, statusExampleApi.readAll());
        QCOMPARE(timelineModel.rowCount({}), 1);

        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::IdRole).value<QString>(), "103270115826048975");
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::MentionsRole).value<QStringList>(), QStringList{});
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::ContentRole).value<QString>(), "<p>LOREM</p>");
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->id(), QStringLiteral("1"));
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->displayNameHtml(),
                 QStringLiteral("Eugen <img height=\"16\" align=\"middle\" width=\"16\" src=\"https://kde.org\">"));
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::IsBoostedRole).value<bool>(), false);

        const auto poll = timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::PollRole).value<Poll>();
        QCOMPARE(poll.id(), "34830");
        QCOMPARE(poll.expiresAt().date().year(), 2019);
        QCOMPARE(poll.expired(), true);
        QCOMPARE(poll.multiple(), false);
        QCOMPARE(poll.votesCount(), 10);
        QCOMPARE(poll.votersCount(), -1);
        QCOMPARE(poll.voted(), true);
        QCOMPARE(poll.ownVotes().count(), 1);
        QCOMPARE(poll.ownVotes()[0], 1);
        QCOMPARE(poll.options().count(), 2);
        QCOMPARE(poll.options()[0]["title"], QStringLiteral("accept"));
        QCOMPARE(poll.options()[0]["votesCount"], 6);
        QCOMPARE(poll.options()[1]["title"], QStringLiteral("deny <img height=\"16\" align=\"middle\" width=\"16\" src=\"https://kde.org\">"));
        QCOMPARE(poll.options()[1]["votesCount"], 4);

        account->registerPost(QString("/api/v1/polls/34830/votes"), new TestReply("poll.json", account));

        QSignalSpy spy(&timelineModel, &QAbstractItemModel::dataChanged);
        QVERIFY(spy.isValid());
        timelineModel.actionVote(timelineModel.index(0, 0), {0});
        spy.wait(1000);
        QCOMPARE(spy.count(), 1);
        const auto arguments = spy.takeFirst();
        QCOMPARE(arguments[0].value<QModelIndex>().row(), 0);
        QCOMPARE(arguments[1].value<QModelIndex>().row(), 0);
        QCOMPARE(arguments[2].value<QVector<int>>().count(), 1);
        QCOMPARE(arguments[2].value<QVector<int>>()[0], AbstractTimelineModel::PollRole);
    }

private:
    MockAccount *account;
};

QTEST_MAIN(TimelineTest)
#include "timelinetest.moc"
