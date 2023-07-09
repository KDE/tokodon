// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include "utils/blurhash.h"
#include "utils/blurhashimageprovider.h"

class BlurHashTest : public QObject
{
    Q_OBJECT

    QImage goodBlurHashImage;
    QImage specialBlurHashImage;
    BlurhashImageProvider *imageProvider;

private Q_SLOTS:
    void initTestCase()
    {
        goodBlurHashImage = QImage(QLatin1String(DATA_DIR) + QLatin1String("/blurhash.png")).convertToFormat(QImage::Format_RGB888);
        specialBlurHashImage = QImage(QLatin1String(DATA_DIR) + QLatin1String("/blurhash2.png")).convertToFormat(QImage::Format_RGB888);
        imageProvider = new BlurhashImageProvider();
    }

    void testImageProviderEncoding()
    {
        const QString blurHashEncoded = QStringLiteral(
            "%7CKO2%3FU%252Tw%3DwR6cErDEhOD%5D%7ERBVZRip0W9ofwxM_%7D;RPxuwH%253s89%5Dt8%24%25tLOtxZ%25gixtQt8IUS%23I.ENa0NZIVt6xFM%7BM%7B%251j%5EM_bcRPX9nht7n%"
            "2Bj%5BrrW;ni%25Mt7V%40W;t7t8%251bbxat7WBIUR%2ARjRjRjxuRjs.MxbbV%40WY");

        auto response = dynamic_cast<AsyncImageResponse *>(imageProvider->requestImageResponse(blurHashEncoded, QSize(25, 25)));
        QSignalSpy spy(response, &QQuickImageResponse::finished);
        spy.wait();

        QCOMPARE(response->m_image.size(), QSize(25, 25));
        QCOMPARE(response->m_image, goodBlurHashImage);
    }

    void testImageProviderSpecialEncoding()
    {
        const QString blurHashEncoded = QStringLiteral("URI#cIR+L1%14eoJtAWYXMt5IAob4oRQfiRR");

        auto response = dynamic_cast<AsyncImageResponse *>(imageProvider->requestImageResponse(blurHashEncoded, QSize(25, 25)));
        QSignalSpy spy(response, &QQuickImageResponse::finished);
        spy.wait();

        QCOMPARE(response->m_image.size(), QSize(25, 25));
        QCOMPARE(response->m_image, specialBlurHashImage);
    }

    void testBlurHashAlgo()
    {
        const QByteArray blurHash = QByteArrayLiteral(
            "|KO2?U%2Tw=wR6cErDEhOD]~RBVZRip0W9ofwxM_};RPxuwH%3s89]t8$%tLOtxZ%gixtQt8IUS#I.ENa0NZIVt6xFM{M{%1j^M_bcRPX9nht7n+j[rrW;ni%Mt7V@W;t7t8%1bbxat7WBIUR*"
            "RjRjRjxuRjs.MxbbV@WY");

        auto data = decode(blurHash.constData(), 25, 25, 1, 3);
        QImage image(data, 25, 25, 25 * 3, QImage::Format_RGB888, free, data);

        QCOMPARE(image, goodBlurHashImage);
    }
};

QTEST_MAIN(BlurHashTest)
#include "blurhashtest.moc"
