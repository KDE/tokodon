// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstraste.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/socialgraphmodel.h"
#include "account/accountmanager.h"
#include "autotests/helperreply.h"
#include "autotests/mockaccount.h"

#include <QtTest/QtTest>

class SocialGraphModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        AccountManager::instance().setTestMode(true);

        account = new MockAccount();

        QUrl url = account->apiUrl(QStringLiteral("/api/v1/follow_requests"));
        account->registerGet(url, new TestReply(QStringLiteral("socialgraphmodel_follows.json"), account));

        url = account->apiUrl(QStringLiteral("/api/v1/accounts/mock/followers"));
        account->registerGet(url, new TestReply(QStringLiteral("socialgraphmodel_follows.json"), account));

        url = account->apiUrl(QStringLiteral("/api/v1/accounts/mock/following"));
        account->registerGet(url, new TestReply(QStringLiteral("socialgraphmodel_follows.json"), account));

        url = account->apiUrl(QStringLiteral("/api/v1/mutes"));
        account->registerGet(url, new TestReply(QStringLiteral("socialgraphmodel_follows.json"), account));

        url = account->apiUrl(QStringLiteral("/api/v1/blocks"));
        account->registerGet(url, new TestReply(QStringLiteral("socialgraphmodel_follows.json"), account));

        url = account->apiUrl(QStringLiteral("/api/v1/endorsements"));
        account->registerGet(url, new TestReply(QStringLiteral("socialgraphmodel_follows.json"), account));

        url = account->apiUrl(QStringLiteral("/api/v1/statuses/mock/favourited_by"));
        account->registerGet(url, new TestReply(QStringLiteral("socialgraphmodel_follows.json"), account));

        url = account->apiUrl(QStringLiteral("/api/v1/statuses/mock/reblogged_by"));
        account->registerGet(url, new TestReply(QStringLiteral("socialgraphmodel_follows.json"), account));

        url = account->apiUrl(QStringLiteral("/api/v1/accounts/familiar_followers"));
        account->registerGet(url, new TestReply(QStringLiteral("socialgraphmodel_follows.json"), account));

        AccountManager::instance().addAccount(account, false);
        AccountManager::instance().selectAccount(account, false);
    }

    void testInvalid()
    {
        {
            SocialGraphModel socialGraphModel;
            QCOMPARE(socialGraphModel.rowCount({}), 0);
        }

        // accountId with no name shouldn't load
        {
            SocialGraphModel socialGraphModel;
            socialGraphModel.setAccountId(QStringLiteral("mock"));
            QCOMPARE(socialGraphModel.rowCount({}), 0);
        }

        // name without an accountId or statusId shouldn't load
        {
            SocialGraphModel socialGraphModel;
            socialGraphModel.setName(QStringLiteral("following"));
            QCOMPARE(socialGraphModel.rowCount({}), 0);
        }
    }

    void testModel_data()
    {
        QTest::addColumn<QString>("name");
        QTest::addColumn<bool>("status");

        QTest::addRow("request") << "request" << false;
        QTest::addRow("followers") << "followers" << false;
        QTest::addRow("following") << "following" << false;
        QTest::addRow("mutes") << "mutes" << false;
        QTest::addRow("blocks") << "blocks" << false;
        QTest::addRow("featured") << "featured" << false;
        QTest::addRow("favourited_by") << "favourited_by" << true;
        QTest::addRow("reblogged_by") << "reblogged_by" << true;
    }

    void testModel()
    {
        QFETCH(QString, name);
        QFETCH(bool, status);

        SocialGraphModel socialGraphModel;
        socialGraphModel.setName(name);
        if (status) {
            socialGraphModel.setStatusId(QStringLiteral("mock"));
        } else {
            socialGraphModel.setAccountId(QStringLiteral("mock"));
        }
        QCOMPARE(socialGraphModel.rowCount({}), 2);

        auto firstIdentity = socialGraphModel.data(socialGraphModel.index(0, 0), SocialGraphModel::IdentityRole).value<Identity *>();
        QVERIFY(firstIdentity != nullptr);
        QCOMPARE(firstIdentity->id(), QStringLiteral("1"));
        QCOMPARE(firstIdentity->username(), QStringLiteral("Gargron"));

        auto secondIdentity = socialGraphModel.data(socialGraphModel.index(1, 0), SocialGraphModel::IdentityRole).value<Identity *>();
        QVERIFY(secondIdentity != nullptr);
        QCOMPARE(secondIdentity->id(), QStringLiteral("2"));
        QCOMPARE(secondIdentity->username(), QStringLiteral("SGargron"));
    }

private:
    MockAccount *account;
};

QTEST_MAIN(SocialGraphModelTest)
#include "socialgraphmodeltest.moc"
