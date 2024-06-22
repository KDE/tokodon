// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "autotests/helperreply.h"
#include "autotests/mockaccount.h"
#include "search/searchmodel.h"

class SearchTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        account = new MockAccount();
        AccountManager::instance().addAccount(account, false);
        AccountManager::instance().selectAccount(account, false);
    }

    void testModel()
    {
        QUrl url = account->apiUrl(QStringLiteral("/api/v2/search"));
        url.setQuery(QUrlQuery{{QStringLiteral("q"), QStringLiteral("myQuery")}, {QStringLiteral("resolve"), QStringLiteral("true")}});
        account->registerGet(url, new TestReply(QStringLiteral("search-result.json"), account));

        SearchModel searchModel;
        searchModel.search(QStringLiteral("myQuery"));

        QCOMPARE(searchModel.rowCount({}), 3);
        QCOMPARE(searchModel.data(searchModel.index(0, 0), AbstractTimelineModel::TypeRole), SearchModel::Account);
        QCOMPARE(searchModel.data(searchModel.index(1, 0), AbstractTimelineModel::TypeRole), SearchModel::Status);
        QCOMPARE(searchModel.data(searchModel.index(0, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->avatarUrl(),
                 QUrl(QStringLiteral("https://files.mastodon.social/accounts/avatars/000/000/001/original/d96d39a0abb45b92.jpg")));
        QCOMPARE(searchModel.data(searchModel.index(1, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->avatarUrl(),
                 QUrl(QStringLiteral("https://files.mastodon.social/accounts/avatars/000/000/001/original/d96d39a0abb45b92.jpg")));
    }

private:
    MockAccount *account = nullptr;
};

QTEST_MAIN(SearchTest)
#include "searchtest.moc"
