// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QtTest/QtTest>

#include "account/accountmanager.h"
#include "account/profileeditor.h"
#include "autotests/helperreply.h"
#include "autotests/mockaccount.h"

class ProfileEditorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        AccountManager::instance().setTestMode(true);
        account = new MockAccount();
        AccountManager::instance().addAccount(account);
    }

    void loadDataTest()
    {
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/accounts/verify_credentials")),
                             new TestReply(QStringLiteral("verify_credentials.json"), account));

        ProfileEditorBackend backend;
        account->setUsername(QStringLiteral("trwnh"));
        backend.setAccount(account);
        QCOMPARE(backend.account(), account);
        QCOMPARE(backend.displayName(), QStringLiteral("infinite love ⴳ"));
        QCOMPARE(
            backend.note(),
            QStringLiteral(
                "i have approximate knowledge of many things. perpetual student. (nb/ace/they)\r\n\r\nxmpp/email: a@trwnh.com\r\nhttps://trwnh.com\r\nhelp me "
                "live: https://liberapay.com/at or https://paypal.me/trwnh\r\n\r\n- my triggers are moths and glitter\r\n- i have all notifs except mentions "
                "turned off, so please interact if you wanna be friends! i literally will not notice otherwise\r\n- dm me if i did something wrong, so i can "
                "improve\r\n- purest person on fedi, do not lewd in my presence\r\n- #1 ami cole fan account\r\n\r\n:fatyoshi:"));
        QCOMPARE(backend.bot(), false);
        QCOMPARE(backend.avatarUrl(), QUrl(QStringLiteral("https://files.mastodon.social/accounts/avatars/000/014/715/original/34aa222f4ae2e0a9.png")));
        QCOMPARE(backend.backgroundUrl(), QUrl(QStringLiteral("https://files.mastodon.social/accounts/headers/000/014/715/original/5c6fc24edb3bb873.jpg")));
    }

    void setDataTest()
    {
        account->registerGet(account->apiUrl(QStringLiteral("/api/v1/accounts/verify_credentials")),
                             new TestReply(QStringLiteral("verify_credentials.json"), account));
        ProfileEditorBackend backend;
        account->setUsername(QStringLiteral("trwnh"));
        backend.setAccount(account);
        backend.setDisplayName(QStringLiteral("Hello"));
        backend.setAvatarUrl(QUrl(QLatin1String(DATA_DIR "/test.png")));
        backend.setBackgroundUrl(QUrl(QLatin1String(DATA_DIR "/test.png")));
        QCOMPARE(QStringLiteral("Hello"), backend.displayName());
        QCOMPARE(QUrl(QLatin1String(DATA_DIR "/test.png")), backend.avatarUrl());
        QCOMPARE(QUrl(QLatin1String(DATA_DIR "/test.png")), backend.backgroundUrl());
        QCOMPARE(QString(), backend.backgroundUrlError());
        QCOMPARE(QString(), backend.avatarUrlError());
    }

private:
    MockAccount *account = nullptr;
};

QTEST_MAIN(ProfileEditorTest)
#include "profileeditortest.moc"
