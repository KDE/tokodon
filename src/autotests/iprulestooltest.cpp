// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "account/accountmanager.h"
#include "admin/iprulestoolmodel.h"
#include "helperreply.h"
#include "mockaccount.h"

#include <QAbstractItemModelTester>
#include <QSignalSpy>
#include <QtTest/QtTest>

class IpRulesToolTest : public QObject
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
        QUrl url = account->apiUrl(QStringLiteral("/api/v1/admin/ip_blocks"));
        account->registerGet(url, new TestReply(QStringLiteral("ip-info.json"), account));

        IpRulesToolModel ipRulesToolModel;
        QCOMPARE(ipRulesToolModel.rowCount({}), 4);
        QCOMPARE(ipRulesToolModel.data(ipRulesToolModel.index(0, 0), IpRulesToolModel::IdRole).toInt(), 1);
        QCOMPARE(ipRulesToolModel.data(ipRulesToolModel.index(0, 0), IpRulesToolModel::IpRole).toString(), QStringLiteral("192.0.2.0/30"));
        Q_ASSERT(ipRulesToolModel.data(ipRulesToolModel.index(0, 0), IpRulesToolModel::SeverityRole).isValid());
        QCOMPARE(ipRulesToolModel.data(ipRulesToolModel.index(0, 0), IpRulesToolModel::SeverityRole).toInt(), IpInfo::BlockAccess);
        QCOMPARE(ipRulesToolModel.data(ipRulesToolModel.index(0, 0), IpRulesToolModel::CommentRole).toString(), QStringLiteral("konqi is cute"));
        Q_ASSERT(ipRulesToolModel.data(ipRulesToolModel.index(0, 0), IpRulesToolModel::CreatedAtRole).isValid());
        Q_ASSERT(ipRulesToolModel.data(ipRulesToolModel.index(0, 0), IpRulesToolModel::ExpiredAtRole).isValid());
    }

private:
    MockAccount *account = nullptr;
};

QTEST_MAIN(IpRulesToolTest)
#include "iprulestooltest.moc"