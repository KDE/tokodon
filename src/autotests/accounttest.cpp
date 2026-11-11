// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstraste.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/accountmanager.h"
#include "account/announcementmodel.h"
#include "autotests/helperreply.h"
#include "autotests/mockaccount.h"

#include <QtTest/QtTest>

#include <limits>

class AccountTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        AccountManager::instance().setTestMode(true);
        account = new MockAccount();
        AccountManager::instance().addAccount(account);
    }

    // Ensure the settings group is named correctly
    void settingsNames()
    {
        account->setUsername(QStringLiteral("foo"));
        account->setInstanceUri(QStringLiteral("kde.social"));

        QCOMPARE(account->settingsGroupName(), QStringLiteral("foo@kde.social"));
        QCOMPARE(account->clientSecretKey(), QStringLiteral("foo@kde.social-client-secret"));
        QCOMPARE(account->accessTokenKey(), QStringLiteral("foo@kde.social-access-token"));
    }

    // Make sure we can parse v1 instance information
    void testApiV1InstanceInfo()
    {
        const QSignalSpy spy(account, &AbstractAccount::fetchedInstanceMetadata);

        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/instance")), new TestReply(QStringLiteral("api_v1_instance.json"), this));
        account->fetchInstanceMetadata();

        QCOMPARE(spy.count(), 1);
        QCOMPARE(account->instanceName(), QStringLiteral("Mastodon"));
        QCOMPARE(account->maxPostLength(), 500);
        QCOMPARE(account->charactersReservedPerUrl(), 23);
        QCOMPARE(account->maxPollOptions(), 4);
        QCOMPARE(account->registrationsOpen(), true);
        QCOMPARE(account->registrationMessage(), QString());
        QCOMPARE(account->supportsLocalVisibility(), false);
    }

    // Make sure we can parse v2 instance information
    void testApiV2InstanceInfo()
    {
        const QSignalSpy spy(account, &AbstractAccount::fetchedInstanceMetadata);

        account->registerGet(account->apiUrl(QStringLiteral("/api/v2/instance")), new TestReply(QStringLiteral("api_v2_instance.json"), this));
        account->fetchInstanceMetadata();

        QCOMPARE(spy.count(), 1);
        QCOMPARE(account->instanceName(), QStringLiteral("Mastodon"));
        QCOMPARE(account->maxPostLength(), 500);
        QCOMPARE(account->charactersReservedPerUrl(), 23);
        QCOMPARE(account->maxPollOptions(), 4);
        QCOMPARE(account->registrationsOpen(), true);
        QCOMPARE(account->registrationMessage(), QString());
        QCOMPARE(account->supportsLocalVisibility(), false);
    }

    // Instances can be configured with absurd character limits that don't fit in an int, make sure we don't treat them as zero
    void testHugeCharacterLimit()
    {
        const QSignalSpy spy(account, &AbstractAccount::fetchedInstanceMetadata);

        account->registerGet(account->apiUrl(QStringLiteral("/api/v2/instance")),
                             new TestReply(QStringLiteral("api_v2_instance_huge_character_limit.json"), this));
        account->fetchInstanceMetadata();

        QCOMPARE(spy.count(), 1);
        QCOMPARE(account->maxPostLength(), static_cast<size_t>(std::numeric_limits<int>::max()));
    }

private:
    MockAccount *account;
};

QTEST_MAIN(AccountTest)
#include "accounttest.moc"
