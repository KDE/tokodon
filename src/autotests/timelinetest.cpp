// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "mockaccount.h"
#include "post.h"
#include "timelinemodel.h"
#include <QAbstractItemModelTester>
#include <QSignalSpy>

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
};

QTEST_MAIN(TimelineTest)
#include "timelinetest.moc"
