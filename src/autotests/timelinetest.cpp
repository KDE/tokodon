// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "mockaccount.h"
#include "post.h"
#include "poll.h"
#include "timelinemodel.h"
#include <QAbstractItemModelTester>
#include <QSignalSpy>

class PollReply : public QNetworkReply
{
public:
    PollReply(QObject *parent)
        : QNetworkReply(parent)
    {
        setError(NetworkError::NoError, QString());
        setFinished(true);

        pollExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + "poll.json");
        pollExampleApi.open(QIODevice::ReadOnly);
    }

    virtual qint64 readData(char *data, qint64 maxSize) override {
        return pollExampleApi.read(data, maxSize);
    }

    virtual void abort() override {
    }

    QFile pollExampleApi;
};

class TimelineTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
    }

    void testModel()
    {
        auto account = new MockAccount();
        AccountManager::instance().addAccount(account);
        AccountManager::instance().selectAccount(account);

        TimelineModel timelineModel;
        timelineModel.setAccountManager(&AccountManager::instance());
        timelineModel.setName("home");

        QFile statusExampleApi;
        statusExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + "status.json");
        statusExampleApi.open(QIODevice::ReadOnly);
        const auto doc = QJsonDocument::fromJson(statusExampleApi.readAll());
        auto post = new Post(account, doc.object(), this);
        timelineModel.fetchedTimeline(account, "home", {post});
        QCOMPARE(timelineModel.rowCount({}), 1);
    }

    void testModelPoll()
    {
        auto account = new MockAccount();
        AccountManager::instance().addAccount(account);
        AccountManager::instance().selectAccount(account);

        TimelineModel timelineModel;
        timelineModel.setAccountManager(&AccountManager::instance());
        timelineModel.setName("home");

        QFile statusExampleApi;
        statusExampleApi.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + "status-poll.json");
        statusExampleApi.open(QIODevice::ReadOnly);
        const auto doc = QJsonDocument::fromJson(statusExampleApi.readAll());
        auto post = new Post(account, doc.object(), this);
        timelineModel.fetchedTimeline(account, "home", {post});
        QCOMPARE(timelineModel.rowCount({}), 1);

        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::IdRole).value<QString>(), "103270115826048975");
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::MentionsRole).value<QStringList>(), QStringList{});
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), Qt::DisplayRole).value<QString>(), "<p>LOREM</p>");
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::AuthorIdRole).value<QString>(), QStringLiteral("Gargron"));
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::AuthorDisplayNameRole).value<QString>(), QStringLiteral("Eugen <img height=\"16\" align=\"middle\" width=\"16\" src=\"https://kde.org\">"));
        QCOMPARE(timelineModel.data(timelineModel.index(0, 0), AbstractTimelineModel::WasRebloggedRole).value<bool>(), false);

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

        account->registerPost(QString("/api/v1/polls/34830/votes"), new PollReply(account));

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
};

QTEST_MAIN(TimelineTest)
#include "timelinetest.moc"
