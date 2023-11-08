// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstraste.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/accountmanager.h"
#include "account/announcementmodel.h"
#include "helperreply.h"
#include "mockaccount.h"

#include <QSignalSpy>
#include <QtTest/QtTest>

class AnnouncementsTest : public QObject
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
        QUrl url = account->apiUrl(QStringLiteral("/api/v1/announcements"));
        account->registerGet(url, new TestReply(QStringLiteral("announcements.json"), account));

        AnnouncementModel announcementsModel;
        QCOMPARE(announcementsModel.rowCount({}), 1);
        QCOMPARE(announcementsModel.data(announcementsModel.index(0, 0), AnnouncementModel::IdRole).toInt(), 8);
        QCOMPARE(announcementsModel.data(announcementsModel.index(0, 0), AnnouncementModel::ContentRole).toString(),
                 QStringLiteral("<p>Looks like there was an issue processing audio attachments without embedded art since yesterday due to an experimental new "
                                "feature. That issue has now been fixed, so you may see older posts with audio from other servers pop up in your feeds now as "
                                "they are being finally properly processed. Sorry!</p>"));
        Q_ASSERT(announcementsModel.data(announcementsModel.index(0, 0), AnnouncementModel::PublishedAt).toDate().isValid());
    }

private:
    MockAccount *account;
};

QTEST_MAIN(AnnouncementsTest)
#include "announcementstest.moc"
