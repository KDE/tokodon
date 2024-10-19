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
};

QTEST_MAIN(TextHandlerTest)
#include "texthandlertest.moc"
