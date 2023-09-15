// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/accountmanager.h"
#include "admin/federationtoolmodel.h"
#include "helperreply.h"
#include "mockaccount.h"

#include <QAbstractItemModelTester>
#include <QSignalSpy>
#include <QtTest/QtTest>

class FederationToolTest : public QObject
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
        QUrl url = account->apiUrl("/api/v1/admin/domain_blocks");
        account->registerGet(url, new TestReply("federation-info.json", account));

        FederationToolModel federationToolModel;
        QCOMPARE(federationToolModel.rowCount({}), 2);
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::IdRole).toInt(), 1);
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::DomainRole).toUrl(), QUrl("kde.org"));
        Q_ASSERT(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::CreatedAtRole).isValid());
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::SeverityRole).toString(), "silence");
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::RejectMediaRole).toBool(), false);
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::RejectReportsRole).toBool(), false);
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::PrivateCommentRole).toString(), "This is a private comment");
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::PublicCommentRole).toString(), "This is a public comment");
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::ObfuscateRole).toBool(), false);
    }

private:
    MockAccount *account;
};

QTEST_MAIN(FederationToolTest)
#include "federationtooltest.moc"
