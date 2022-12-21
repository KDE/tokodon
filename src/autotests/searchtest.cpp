// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "mockaccount.h"
#include "post.h"
#include "poll.h"
#include "searchmodel.h"
#include "helperreply.h"
#include <QAbstractItemModelTester>
#include <QSignalSpy>

class SearchTest : public QObject
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
        QUrl url = account->apiUrl("/api/v2/search");
        url.setQuery(QUrlQuery{{"q", "myQuery"}});
        account->registerGet(url, new TestReply("search-result.json", account));

        SearchModel searchModel;
        searchModel.search("myQuery");

        QCOMPARE(searchModel.rowCount({}), 2);
        QCOMPARE(searchModel.data(searchModel.index(0, 0), AbstractTimelineModel::TypeRole), SearchModel::Account);
        QCOMPARE(searchModel.data(searchModel.index(1, 0), AbstractTimelineModel::TypeRole), SearchModel::Status);
        QCOMPARE(searchModel.data(searchModel.index(0, 0), AbstractTimelineModel::AvatarRole), QUrl("https://files.mastodon.social/accounts/avatars/000/000/001/original/d96d39a0abb45b92.jpg"));
        QCOMPARE(searchModel.data(searchModel.index(1, 0), AbstractTimelineModel::AvatarRole), QUrl("https://files.mastodon.social/accounts/avatars/000/000/001/original/d96d39a0abb45b92.jpg"));
    }
};

QTEST_MAIN(SearchTest)
#include "searchtest.moc"
