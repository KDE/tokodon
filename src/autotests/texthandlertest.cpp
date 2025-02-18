// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "utils/texthandler.h"

class TextHandlerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void getNextPrevLinks_data()
    {
        using URL = std::optional<QUrl>;

        QTest::addColumn<QString>("header");
        QTest::addColumn<URL>("next");
        QTest::addColumn<URL>("prev");

        QTest::addRow("next/prev") << QStringLiteral("<https://kde.org/>; rel=\"next\", <https://gnome.org/>; rel=\"prev\"")
                                   << URL(QStringLiteral("https://kde.org/")) << URL(QStringLiteral("https://gnome.org/"));
        QTest::addRow("next") << QStringLiteral("<https://kde.org/>; rel=\"next\", <>; rel=\"prev\"") << URL(QStringLiteral("https://kde.org/"))
                              << URL(std::nullopt);
        QTest::addRow("prev") << QStringLiteral("<>; rel=\"next\", <https://gnome.org/>; rel=\"prev\"") << URL(std::nullopt)
                              << URL(QStringLiteral("https://gnome.org/"));
    }

    void getNextPrevLinks()
    {
        QFETCH(QString, header);
        QFETCH(std::optional<QUrl>, next);
        QFETCH(std::optional<QUrl>, prev);

        QCOMPARE(TextHandler::getNextLink(header), next);
        QCOMPARE(TextHandler::getPrevLink(header), prev);
    }

    void isPostUrl_data()
    {
        QTest::addColumn<QString>("url");
        QTest::addColumn<bool>("valid");

        QTest::addRow("mastodon post") << QStringLiteral("https://floss.social/@kde/114020671121601181") << true;
        QTest::addRow("mastodon profile") << QStringLiteral("https://floss.social/@kde") << false;
        QTest::addRow("mastodon instance") << QStringLiteral("https://floss.social") << false;

        QTest::addRow("pleroma/akkoma post") << QStringLiteral("https://hi.nighten.fr/notice/AqkP9JecfrRF1EPjns") << true;
        QTest::addRow("pleroma/akkoma post") << QStringLiteral("https://hi.nighten.fr/objects/AqkP9JecfrRF1EPjns") << true;

        QTest::addRow("sharkey/misskey post") << QStringLiteral("https://transfem.social/notes/9mp8p53p4g4p00vt") << true;
    }

    void isPostUrl()
    {
        QFETCH(QString, url);
        QFETCH(bool, valid);

        QCOMPARE(TextHandler::isPostUrl(url), valid);
    }
};

QTEST_MAIN(TextHandlerTest)
#include "texthandlertest.moc"
