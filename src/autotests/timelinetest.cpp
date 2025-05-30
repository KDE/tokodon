// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "autotests/helperreply.h"
#include "autotests/mockaccount.h"
#include "timeline/maintimelinemodel.h"
#include "timeline/tagstimelinemodel.h"
#include "timeline/threadmodel.h"
#include "utils/texthandler.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

class TimelineTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        AccountManager::instance().setTestMode(true);
        account = new MockAccount();
        AccountManager::instance().addAccount(account);
    }

    void testMainDisplayName()
    {
        KLocalizedString::setApplicationDomain(QByteArrayLiteral("tokodon"));
        KLocalizedString::setLanguages(QStringList{QStringLiteral("C")});
        account->setUsername(QStringLiteral("test"));

        QJsonObject fakeIdentity;
        fakeIdentity["id"_L1] = QStringLiteral("1");
        fakeIdentity["display_name"_L1] = QStringLiteral("test");

        account->setFakeIdentity(fakeIdentity);

        MainTimelineModel timelineModel;
        timelineModel.setName(QStringLiteral("public"));
        QCOMPARE(timelineModel.displayName(), QStringLiteral("Local"));
        timelineModel.setName(QStringLiteral("federated"));
        QCOMPARE(timelineModel.displayName(), QStringLiteral("Global"));
        timelineModel.setName(QStringLiteral("home"));
        QCOMPARE(timelineModel.displayName(), QStringLiteral("Home"));

        account->clearFakeIdentity();
    }

    void testStreamUpdate()
    {
        QFile statusExampleApi;
        statusExampleApi.setFileName(QLatin1String(DATA_DIR "/status.json"));
        statusExampleApi.open(QIODevice::ReadOnly);

        MainTimelineModel timelineModel;
        timelineModel.setName(QStringLiteral("home"));
        QCOMPARE(timelineModel.rowCount({}), 0);

        account->streamingEvent(AbstractAccount::StreamingEventType::UpdateEvent, statusExampleApi.readAll());
        QCOMPARE(timelineModel.rowCount({}), 1);
    }

    void testFillTimelineMain()
    {
        QUrl markersUrl = account->apiUrl(QStringLiteral("/api/v1/markers"));
        markersUrl.setQuery(QStringLiteral("timeline[]=home"));
        account->registerGet(markersUrl, new TestReply(QStringLiteral("markers.json"), account));
        auto fetchMoreUrl = account->apiUrl(QStringLiteral("/api/v1/timelines/home"));
        fetchMoreUrl.setQuery(QUrlQuery{
            {QStringLiteral("max_id"), QStringLiteral("103270115826038975")},
        });
        account->registerGet(fetchMoreUrl, new TestReply(QStringLiteral("statuses.json"), account));

        auto statusReply = new TestReply(QStringLiteral("statuses.json"), account);
        statusReply->setRawHeader("Link", QStringLiteral("<%1>; rel=\"next\", <>; rel=\"prev\"").arg(fetchMoreUrl.toString()).toUtf8());
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/timelines/home")), statusReply);

        MainTimelineModel timelineModel;
        timelineModel.setName(QStringLiteral("home"));

        QCOMPARE(timelineModel.rowCount({}), 5);
        QVERIFY(timelineModel.canFetchMore({}));

        // TODO: this is supposed to be 10, but now due to post id de-duplication this now works as a test of that. that's not intentional
        timelineModel.fetchMore({});
        QCOMPARE(timelineModel.rowCount({}), 5);
    }

    void testTagModel()
    {
        auto fetchMoreUrl = account->apiUrl(QStringLiteral("/api/v1/timelines/tag/home"));
        fetchMoreUrl.setQuery(QUrlQuery{
            {QStringLiteral("max_id"), QStringLiteral("103270115826038975")},
        });
        auto statusReply = new TestReply(QStringLiteral("statuses.json"), account);
        statusReply->setRawHeader("Link", QStringLiteral("<%1>; rel=\"next\", <>; rel=\"prev\"").arg(fetchMoreUrl.toString()).toUtf8());
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/timelines/tag/home")), statusReply);
        account->registerGet(fetchMoreUrl, new TestReply(QStringLiteral("statuses.json"), account));

        TagsTimelineModel tagModel;
        tagModel.setHashtag(QStringLiteral("home"));

        QCOMPARE(tagModel.rowCount({}), 5);
        QVERIFY(tagModel.canFetchMore({}));

        // TODO: this is supposed to be 10, but now due to post id de-duplication this now works as a test of that. that's not intentional
        tagModel.fetchMore({});
        QCOMPARE(tagModel.rowCount({}), 5);
    }

    void testThreadModel()
    {
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/statuses/103270115826048975")), new TestReply(QStringLiteral("status.json"), account));
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/statuses/103270115826048975/context")),
                             new TestReply(QStringLiteral("context.json"), account));

        ThreadModel threadModel;
        threadModel.setPostId(QStringLiteral("103270115826048975"));
        QCOMPARE(threadModel.rowCount({}), 4);
        QCOMPARE(threadModel.data(threadModel.index(1, 0), AbstractTimelineModel::SelectedRole).toBool(), true);
        QCOMPARE(threadModel.displayName(), QStringLiteral("Post by Eugen :kde:"));
        QCOMPARE(threadModel.postId(), QStringLiteral("103270115826048975"));
        QCOMPARE(threadModel.canFetchMore({}), false);

        // in_reply_to_account_id filled
        QCOMPARE(threadModel.data(threadModel.index(2, 0), AbstractTimelineModel::IsReplyRole).toBool(), true);
        QCOMPARE(threadModel.data(threadModel.index(2, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->username(),
                 QStringLiteral("Gargron"));

        // in_reply_to_account_id unfilled
        QCOMPARE(threadModel.data(threadModel.index(3, 0), AbstractTimelineModel::IsReplyRole).toBool(), true);
        QCOMPARE(threadModel.data(threadModel.index(3, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->username(),
                 QStringLiteral("Gargron"));
    }

    void testModelPoll()
    {
        MainTimelineModel timelineModel;
        timelineModel.setName(QStringLiteral("home"));

        QFile statusExampleApi;
        statusExampleApi.setFileName(QLatin1String(DATA_DIR "/status-poll.json"));
        statusExampleApi.open(QIODevice::ReadOnly);
        account->streamingEvent(AbstractAccount::StreamingEventType::UpdateEvent, statusExampleApi.readAll());
        QCOMPARE(timelineModel.rowCount({}), 6);

        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::IdRole).value<QString>(), QStringLiteral("103270115826048976"));
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::MentionsRole).value<QStringList>(), QStringList{});
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::ContentRole).value<QString>(), QStringLiteral("<p>LOREM</p>"));
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->id(), QStringLiteral("1"));
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->displayNameHtml(),
                 QStringLiteral("Eugen <img height=\"16\" align=\"middle\" width=\"16\" src=\"https://kde.org\">"));
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::IsBoostedRole).value<bool>(), false);

        const auto poll = timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::PollRole).value<Poll>();
        QCOMPARE(poll.id(), QStringLiteral("34830"));
        QCOMPARE(poll.expiresAt().date().year(), 2019);
        QCOMPARE(poll.expired(), true);
        QCOMPARE(poll.multiple(), false);
        QCOMPARE(poll.votesCount(), 10);
        QCOMPARE(poll.votersCount(), -1);
        QCOMPARE(poll.voted(), true);
        QCOMPARE(poll.ownVotes().count(), 1);
        QCOMPARE(poll.ownVotes()[0], 1);
        QCOMPARE(poll.options().count(), 2);
        QCOMPARE(poll.options()[0]["title"_L1], QStringLiteral("accept"));
        QCOMPARE(poll.options()[0]["votesCount"_L1], 6);
        QCOMPARE(poll.options()[1]["title"_L1], QStringLiteral("deny <img height=\"16\" align=\"middle\" width=\"16\" src=\"https://kde.org\">"));
        QCOMPARE(poll.options()[1]["votesCount"_L1], 4);

        account->registerPost(QStringLiteral("/api/v1/polls/34830/votes"), new TestReply(QStringLiteral("poll.json"), account));

        QSignalSpy spy(&timelineModel, &QAbstractItemModel::dataChanged);
        QVERIFY(spy.isValid());
        timelineModel.actionVote(timelineModel.index(0, 0), {0});
        spy.wait(1000);
        QCOMPARE(spy.count(), 1);
        const auto arguments = spy.takeFirst();
        QCOMPARE(arguments[0].value<QModelIndex>().row(), 0);
        QCOMPARE(arguments[1].value<QModelIndex>().row(), 0);
        QCOMPARE(arguments[2].value<QList<int>>().count(), 1);
        QCOMPARE(arguments[2].value<QList<int>>()[0], AbstractTimelineModel::PollRole);
    }

    void testFillListTimeline()
    {
        auto fetchMoreUrl = account->apiUrl(QStringLiteral("/api/v1/timelines/list/1"));
        fetchMoreUrl.setQuery(QUrlQuery{
            {QStringLiteral("max_id"), QStringLiteral("103270115826038975")},
        });
        account->registerGet(fetchMoreUrl, new TestReply(QStringLiteral("statuses.json"), account));

        auto statusReply = new TestReply(QStringLiteral("statuses.json"), account);
        statusReply->setRawHeader("Link", QStringLiteral("<%1>; rel=\"next\", <>; rel=\"prev\"").arg(fetchMoreUrl.toString()).toUtf8());
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/timelines/list/1")), statusReply);

        MainTimelineModel timelineModel;
        timelineModel.setName(QStringLiteral("list"));

        // nothing should be loaded because we didn't give it a list id yet
        QCOMPARE(timelineModel.rowCount({}), 0);

        timelineModel.setListId(QStringLiteral("1"));

        QCOMPARE(timelineModel.rowCount({}), 5);
        QVERIFY(timelineModel.canFetchMore({}));

        // TODO: this is supposed to be 10, but now due to post id de-duplication this now works as a test of that. that's not intentional
        timelineModel.fetchMore({});
        QCOMPARE(timelineModel.rowCount({}), 5);
    }

private:
    MockAccount *account = nullptr;
};

QTEST_MAIN(TimelineTest)
#include "timelinetest.moc"
