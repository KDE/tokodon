// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "account/accountmanager.h"
#include "admin/federationtoolmodel.h"
#include "autotests/helperreply.h"
#include "autotests/mockaccount.h"

#include <QtTest/QtTest>

class FederationToolTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        AccountManager::instance().setTestMode(true);
        account = new MockAccount();
        AccountManager::instance().addAccount(account);
        AccountManager::instance().selectAccount(account, false);
    }

    void testModel()
    {
        QUrl url = account->apiUrl(QStringLiteral("/api/v1/admin/domain_blocks"));
        account->registerGet(url, new TestReply(QStringLiteral("federation-info.json"), account));

        FederationToolModel federationToolModel;
        QCOMPARE(federationToolModel.rowCount({}), 2);
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::IdRole).toInt(), 1);
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::DomainRole).toUrl(), QUrl(QStringLiteral("kde.org")));
        Q_ASSERT(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::CreatedAtRole).isValid());
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::SeverityRole).toString(), QStringLiteral("silence"));
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::RejectMediaRole).toBool(), false);
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::RejectReportsRole).toBool(), false);
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::PrivateCommentRole).toString(),
                 QStringLiteral("This is a private comment"));
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::PublicCommentRole).toString(),
                 QStringLiteral("This is a public comment"));
        QCOMPARE(federationToolModel.data(federationToolModel.index(0, 0), FederationToolModel::ObfuscateRole).toBool(), false);
    }

private:
    MockAccount *account = nullptr;
};

QTEST_MAIN(FederationToolTest)
#include "federationtooltest.moc"
