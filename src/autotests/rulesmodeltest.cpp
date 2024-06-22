// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstraste.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/rulesmodel.h"
#include "account/accountmanager.h"
#include "autotests/helperreply.h"
#include "autotests/mockaccount.h"

#include <QtTest/QtTest>

class RulesModelTest : public QObject
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
        QUrl url = account->apiUrl(QStringLiteral("/api/v1/instance/rules"));
        account->registerGet(url, new TestReply(QStringLiteral("rules.json"), account));

        RulesModel rulesModel;
        rulesModel.setAccount(account);
        QCOMPARE(rulesModel.rowCount({}), 6);
        QCOMPARE(rulesModel.data(rulesModel.index(0, 0), RulesModel::IdRole).toInt(), 1);
        QCOMPARE(rulesModel.data(rulesModel.index(0, 0), RulesModel::TextRole).toString(),
                 QStringLiteral("Sexually explicit or violent media must be marked as sensitive when posting"));

        QCOMPARE(rulesModel.data(rulesModel.index(1, 0), RulesModel::IdRole).toInt(), 2);
        QCOMPARE(rulesModel.data(rulesModel.index(1, 0), RulesModel::TextRole).toString(),
                 QStringLiteral("No racism, sexism, homophobia, transphobia, xenophobia, or casteism"));
    }

private:
    MockAccount *account = nullptr;
};

QTEST_MAIN(RulesModelTest)
#include "rulesmodeltest.moc"
