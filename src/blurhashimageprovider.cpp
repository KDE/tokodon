// SPDX-FileCopyrightText: 2021 Tobias Fella <fella@posteo.de>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "blurhashimageprovider.h"

#include <QImage>
#include <QString>

#include "blurhash.h"

BlurhashImageProvider::BlurhashImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

/*
 * Qt unfortunately re-encodes the base83 string in QML.
 * The only special ASCII characters used in the blurhash base83 string are:
 * #$%*+,-.:;=?@[]^_{|}~
 * QUrl::fromPercentEncoding is too greedy, and spits out invalid characters
 * for parts of valid base83 like %14.
 */
static const QMap<QLatin1String, QLatin1String> knownEncodings = {
        {QLatin1String("%23A"), QLatin1String(":")},
        {QLatin1String("%3F"), QLatin1String("?")},
        {QLatin1String("%23"), QLatin1String("#")},
        {QLatin1String("%5B"), QLatin1String("[")},
        {QLatin1String("%5D"), QLatin1String("]")},
        {QLatin1String("%40"), QLatin1String("@")},
        {QLatin1String("%24"), QLatin1String("$")},
        {QLatin1String("%2A"), QLatin1String("*")},
        {QLatin1String("%2B"), QLatin1String("+")},
        {QLatin1String("%2C"), QLatin1String(",")},
        {QLatin1String("%2D"), QLatin1String("-")},
        {QLatin1String("%2E"), QLatin1String(".")},
        {QLatin1String("%3B"), QLatin1String(";")},
        {QLatin1String("%3D"), QLatin1String("=")},
        {QLatin1String("%25"), QLatin1String("%")},
        {QLatin1String("%5E"), QLatin1String("^")},
        {QLatin1String("%7C"), QLatin1String("|")},
        {QLatin1String("%7B"), QLatin1String("{")},
        {QLatin1String("%7D"), QLatin1String("}")},
};

QImage BlurhashImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (id.isEmpty()) {
        return QImage();
    }
    *size = requestedSize;
    if (size->width() == -1) {
        size->setWidth(256);
    }
    if (size->height() == -1) {
        size->setHeight(256);
    }

    QString decodedId = id;

    QMap<QLatin1String, QLatin1String>::const_iterator i;
    for (i = knownEncodings.constBegin(); i != knownEncodings.constEnd(); ++i)
        decodedId.replace(i.key(), i.value());

    auto data = decode(decodedId.toLatin1().constData(), size->width(), size->height(), 1, 3);
    QImage image(data, size->width(), size->height(), size->width() * 3, QImage::Format_RGB888, free, data);
    return image;
}
