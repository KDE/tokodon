// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstraste.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/listsmodel.h"
#include "account/accountmanager.h"
#include "helperreply.h"
#include "mockaccount.h"

#include <QtTest/QtTest>

class ListsModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        account = new MockAccount();
        AccountManager::instance().addAccount(account, false);
        AccountManager::instance().selectAccount(account);
    }

    void cleanupTestCase()
    {
        AccountManager::instance().removeAccount(account);
    }

    void testModel()
    {
        QUrl url = account->apiUrl(QStringLiteral("/api/v1/lists"));
        account->registerGet(url, new TestReply(QStringLiteral("lists.json"), account));

        ListsModel listsModel;
        QCOMPARE(listsModel.rowCount({}), 2);
        QCOMPARE(listsModel.data(listsModel.index(0, 0), ListsModel::IdRole).toInt(), 12249);
        QCOMPARE(listsModel.data(listsModel.index(0, 0), ListsModel::TitleRole).toString(), QStringLiteral("Friends"));
    }

private:
    MockAccount *account;
};

QTEST_MAIN(ListsModelTest)
#include "listsmodeltest.moc"
