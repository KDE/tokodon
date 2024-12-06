// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QImage>

/**
 * @brief Decodes image to and from the BlurHash format. See https://blurha.sh/.
 *
 * @note This class has been adapted from https://github.com/redstrate/QtBlurHash.
 */
class BlurHash
{
public:
    /** Decodes the @p blurhash string creating an image of @p size.
     * @note Returns a null image if decoding failed.
     */
    static QImage decode(const QString &blurhash, const QSize &size);

protected:
    struct Components {
        int x, y;

        bool operator==(const Components &other) const
        {
            return x == other.x && y == other.y;
        }
    };

    /**
     * @brief This is just for storing intermediary color values.
     * We could do this via QColor, but it doesn't work in floating points natively and thus requires expensive conversions.
     */
    struct ColorF {
        float r = 0.0f, g = 0.0f, b = 0.0f;
    };

    /**
     * @brief Decodes a base 83 string to it's integer value. Returns std::nullopt if there's an invalid character in the blurhash.
     */
    static std::optional<int> decode83(const QString &encodedString);

    /**
     * @brief Unpacks an integer to it's @c Components value.
     */
    static Components unpackComponents(int packedComponents);

    /**
     * @brief Decodes a encoded max AC component value.
     */
    static float decodeMaxAC(int value);

    /**
     * @brief Decodes the average color from the encoded RGB value.
     * @note This returns the color as SRGB.
     */
    static QColor decodeAverageColor(int encodedValue);

    /**
     * @brief Calls pow() with @p exp on @p value, while keeping the sign.
     */
    static float signPow(float value, float exp);

    /**
     * @brief Decodes a encoded AC component value.
     */
    static ColorF decodeAC(int value, float maxAC);

    /**
     * @brief Calculates the weighted sum for @p dimension across @p components.
     */
    static QList<float> calculateWeights(qsizetype dimension, qsizetype components);

    friend class BlurHashTest;
};
