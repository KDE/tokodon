// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: MIT

#include "blurhash.h"

#include <QColorSpace>

// From https://github.com/woltapp/blurhash/blob/master/Algorithm.md#base-83
const static QString b83Characters{QStringLiteral("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#$%*+,-.:;=?@[]^_{|}~")};

const static auto toLinearSRGB = QColorSpace(QColorSpace::SRgb).transformationToColorSpace(QColorSpace::SRgbLinear);
const static auto fromLinearSRGB = QColorSpace(QColorSpace::SRgbLinear).transformationToColorSpace(QColorSpace::SRgb);

QImage BlurHash::decode(const QString &blurhash, const QSize &size)
{
    // 10 is the minimum length of a blurhash string
    if (blurhash.length() < 10)
        return {};

    // First character is the number of components
    const auto components83 = decode83(blurhash.first(1));
    if (!components83.has_value())
        return {};

    const auto components = unpackComponents(*components83);
    const auto minimumSize = 1 + 1 + 4 + (components.x * components.y - 1) * 2;
    if (components.x < 1 || components.y < 1 || blurhash.size() != minimumSize)
        return {};

    // Second character is the maximum AC component value
    const auto maxAC83 = decode83(blurhash.mid(1, 1));
    if (!maxAC83.has_value())
        return {};

    const auto maxAC = decodeMaxAC(*maxAC83);

    // Third character onward is the average color of the image
    const auto averageColor83 = decode83(blurhash.mid(2, 4));
    if (!averageColor83.has_value())
        return {};

    const auto averageColor = toLinearSRGB.map(decodeAverageColor(*averageColor83));

    QList values = {averageColor};

    // Iterate through the rest of the string for the color values
    // Each AC component is two characters each
    for (qsizetype c = 6; c < blurhash.size(); c += 2) {
        const auto acComponent83 = decode83(blurhash.mid(c, 2));
        if (!acComponent83.has_value())
            return {};

        values.append(decodeAC(*acComponent83, maxAC));
    }

    QImage image(size, QImage::Format_RGB888);
    image.setColorSpace(QColorSpace::SRgb);

    const auto basisX = calculateWeights(size.width(), components.x);
    const auto basisY = calculateWeights(size.height(), components.y);

    for (int y = 0; y < size.height(); y++) {
        for (int x = 0; x < size.width(); x++) {
            float linearSumR = 0.0f;
            float linearSumG = 0.0f;
            float linearSumB = 0.0f;

            for (int nx = 0; nx < components.x; nx++) {
                for (int ny = 0; ny < components.y; ny++) {
                    const float basis = basisX[x * components.x + nx] * basisY[y * components.y + ny];

                    linearSumR += values[nx + ny * components.x].redF() * basis;
                    linearSumG += values[nx + ny * components.x].greenF() * basis;
                    linearSumB += values[nx + ny * components.x].blueF() * basis;
                }
            }

            auto linearColor = QColor::fromRgbF(linearSumR, linearSumG, linearSumB);
            image.setPixelColor(x, y, fromLinearSRGB.map(linearColor));
        }
    }

    return image;
}

std::optional<int> BlurHash::decode83(const QString &encodedString)
{
    int temp = 0;
    for (const QChar c : encodedString) {
        const auto index = b83Characters.indexOf(c);
        if (index == -1)
            return std::nullopt;

        temp = temp * 83 + static_cast<int>(index);
    }

    return temp;
}

BlurHash::Components BlurHash::unpackComponents(const int packedComponents)
{
    return {packedComponents % 9 + 1, packedComponents / 9 + 1};
}

float BlurHash::decodeMaxAC(const int value)
{
    return static_cast<float>(value + 1) / 166.f;
}

QColor BlurHash::decodeAverageColor(const int encodedValue)
{
    const int intR = encodedValue >> 16;
    const int intG = (encodedValue >> 8) & 255;
    const int intB = encodedValue & 255;

    return QColor::fromRgb(intR, intG, intB);
}

float BlurHash::signPow(const float value, const float exp)
{
    return std::copysign(std::pow(std::abs(value), exp), value);
}

QColor BlurHash::decodeAC(const int value, const float maxAC)
{
    const auto quantR = value / (19 * 19);
    const auto quantG = (value / 19) % 19;
    const auto quantB = value % 19;

    return QColor::fromRgbF(signPow((static_cast<float>(quantR) - 9) / 9, 2) * maxAC,
                            signPow((static_cast<float>(quantG) - 9) / 9, 2) * maxAC,
                            signPow((static_cast<float>(quantB) - 9) / 9, 2) * maxAC);
}

QList<float> BlurHash::calculateWeights(const qsizetype dimension, const qsizetype components)
{
    QList<float> bases(dimension * components, 0.0f);

    const auto scale = static_cast<float>(M_PI) / static_cast<float>(dimension);
    for (qsizetype x = 0; x < dimension; x++) {
        for (qsizetype nx = 0; nx < components; nx++) {
            bases[x * components + nx] = std::cos(scale * static_cast<float>(nx * x));
        }
    }
    return bases;
}
