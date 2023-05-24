// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QtTest/QtTest>

#include "account/accountmanager.h"
#include "helperreply.h"
#include "mockaccount.h"
#include "editor/posteditorbackend.h"

class PostEditorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        account = new MockAccount();
        AccountManager::instance().addAccount(account);
        AccountManager::instance().selectAccount(account);
    }

    void cleanupTestCase()
    {
        AccountManager::instance().removeAccount(account);
    }

    void loadDataTest()
    {
        PostEditorBackend backend;
        backend.setAccount(account);

        QCOMPARE(backend.account(), account);
        QCOMPARE(backend.charactersLeft(), 500);

        const QString status{"Hello, world!"};
        backend.setStatus(status);

        QCOMPARE(backend.status(), status);
        QCOMPARE(backend.charactersLeft(), 487);

        const QString longStatus{
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Duis ultricies lacus "
            "sed turpis tincidunt. Vestibulum lectus mauris ultrices eros. Tristique senectus et netus et malesuada fames ac turpis. Fringilla ut morbi "
            "tincidunt augue interdum. Ullamcorper dignissim cras tincidunt lobortis feugiat vivamus at augue eget. Sociis natoque penatibus et magnis dis. Et "
            "odio pellentesque diam volutpat commodo. Ligula ullamcorper malesuada proin libero. Curabitur gravida arcu ac tortor dignissim."};
        backend.setStatus(longStatus);

        QCOMPARE(backend.charactersLeft(), -57);

        const QString urlStatus{"Lorem ipsum dolor sit amet, https://www.google.com/"};
        backend.setStatus(urlStatus);

        QCOMPARE(backend.charactersLeft(), 449);

        const QString twoUrlStatus{"https://www.google.com/https://www.google.com/"};
        backend.setStatus(twoUrlStatus);

        QCOMPARE(backend.charactersLeft(), 477);
    }

private:
    MockAccount *account;
};

QTEST_MAIN(PostEditorTest)
#include "posteditortest.moc"
