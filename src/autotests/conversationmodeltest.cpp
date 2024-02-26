// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QtTest/QtTest>

#include "autotests/helperreply.h"
#include "autotests/mockaccount.h"
#include "conversation/conversationmodel.h"

class ConversationModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
    }

    void testModel()
    {
        auto account = new MockAccount();
        AccountManager::instance().addAccount(account, false);
        AccountManager::instance().selectAccount(account);
        QUrl url = account->apiUrl(QStringLiteral("/api/v1/conversations"));
        account->registerGet(url, new TestReply(QStringLiteral("conversation-result.json"), account));

        ConversationModel conversationModel;

        QCOMPARE(conversationModel.rowCount({}), 1);
        QCOMPARE(conversationModel.data(conversationModel.index(0, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->avatarUrl(),
                 QUrl(QStringLiteral("https://files.mastodon.social/accounts/avatars/000/000/001/original/d96d39a0abb45b92.jpg")));
        QCOMPARE(conversationModel.data(conversationModel.index(0, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->account(),
                 QStringLiteral("Gargron"));
        QCOMPARE(conversationModel.data(conversationModel.index(0, 0), ConversationModel::ConversationIdRole).toString(), QStringLiteral("418374"));
        QCOMPARE(conversationModel.data(conversationModel.index(0, 0), ConversationModel::UnreadRole), false);
        QCOMPARE(conversationModel.data(conversationModel.index(0, 0), AbstractTimelineModel::ContentRole), QStringLiteral("LOREM"));
        QCOMPARE(conversationModel.data(conversationModel.index(0, 0), AbstractTimelineModel::AuthorIdentityRole).value<Identity *>()->displayName(),
                 QStringLiteral("Eugen"));
    }
};

QTEST_MAIN(ConversationModelTest)
#include "conversationmodeltest.moc"
