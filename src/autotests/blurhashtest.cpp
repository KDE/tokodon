// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: MIT

#include <blurhash.h>

#include <QTest>

class BlurHashTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void decode83_data();
    void decode83();

    void unpackComponents();

    void decodeMaxAC();

    void decodeAverageColor_data();
    void decodeAverageColor();

    void decodeAC();

    void decodeImage();
};

void BlurHashTest::decode83_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<std::optional<int>>("expected");

    // invalid base83 characters
    QTest::addRow("decoding 1") << "試し" << std::optional<int>(std::nullopt);
    QTest::addRow("decoding 2") << "(" << std::optional<int>(std::nullopt);

    QTest::addRow("decoding 3") << "0" << std::optional(0);
    QTest::addRow("decoding 4") << "L" << std::optional(21);
    QTest::addRow("decoding 5") << "U" << std::optional(30);
    QTest::addRow("decoding 6") << "Y" << std::optional(34);
    QTest::addRow("decoding 7") << "1" << std::optional(1);
}

void BlurHashTest::decode83()
{
    QFETCH(QString, value);
    QFETCH(std::optional<int>, expected);

    QCOMPARE(BlurHash::decode83(value), expected);
}

void BlurHashTest::unpackComponents()
{
    BlurHash::Components components;
    components.x = 6;
    components.y = 6;
    QCOMPARE(BlurHash::unpackComponents(50), components);
}

void BlurHashTest::decodeMaxAC()
{
    QCOMPARE(BlurHash::decodeMaxAC(50), 0.307229f);
}

void BlurHashTest::decodeAverageColor_data()
{
    QTest::addColumn<int>("value");
    QTest::addColumn<QColor>("expected");

    QTest::addRow("decoding 1") << 12688010 << QColor(0xffc19a8a);
    QTest::addRow("decoding 2") << 9934485 << QColor(0xff979695);
    QTest::addRow("decoding 3") << 8617624 << QColor(0xff837e98);
    QTest::addRow("decoding 4") << 14604757 << QColor(0xffded9d5);
    QTest::addRow("decoding 5") << 13742755 << QColor(0xffd1b2a3);
}

void BlurHashTest::decodeAverageColor()
{
    QFETCH(int, value);
    QFETCH(QColor, expected);

    QCOMPARE(BlurHash::decodeAverageColor(value), expected);
}

void BlurHashTest::decodeAC()
{
    constexpr auto maxAC = 0.289157f;
    QCOMPARE(BlurHash::decodeAC(0, maxAC), QColor::fromRgbF(-0.289063f, -0.289063f, -0.289063f));
}

void BlurHashTest::decodeImage()
{
    const auto image = BlurHash::decode(QStringLiteral("eBB4=;054UK$=402%s%|r^O%06#?*7RijMxGpYMzniVNT@rFN3#=Kt"), QSize(50, 50));
    QVERIFY(!image.isNull());

    QCOMPARE(image.width(), 50);
    QCOMPARE(image.height(), 50);
    QCOMPARE(image.pixelColor(0, 0), QColor(0xff005f00));
    QCOMPARE(image.pixelColor(30, 30), QColor(0xff99b76d));
}

QTEST_GUILESS_MAIN(BlurHashTest)
#include "blurhashtest.moc"