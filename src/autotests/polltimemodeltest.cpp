// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstraste.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "editor/polltimemodel.h"

#include <QtTest/QtTest>

class PollTimeModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testModel()
    {
        PollTimeModel pollTimeModel;
        QCOMPARE(pollTimeModel.rowCount({}), 8);
        QCOMPARE(pollTimeModel.data(pollTimeModel.index(0, 0), PollTimeModel::TimeRole).toInt(), 300);
        QCOMPARE(pollTimeModel.data(pollTimeModel.index(0, 0), PollTimeModel::TextRole).toString(), QStringLiteral("5 minutes"));

        QCOMPARE(pollTimeModel.data(pollTimeModel.index(3, 0), PollTimeModel::TimeRole).toInt(), 26000);
        QCOMPARE(pollTimeModel.data(pollTimeModel.index(3, 0), PollTimeModel::TextRole).toString(), QStringLiteral("6 hours"));

        QCOMPARE(pollTimeModel.data(pollTimeModel.index(7, 0), PollTimeModel::TimeRole).toInt(), 604800);
        QCOMPARE(pollTimeModel.data(pollTimeModel.index(7, 0), PollTimeModel::TextRole).toString(), QStringLiteral("7 days"));
    }
};

QTEST_MAIN(PollTimeModelTest)
#include "polltimemodeltest.moc"
