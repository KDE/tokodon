// SPDX-FileCopyrightText: 2022 DeepBlueV7.X <https://github.com/deepbluev7>
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <string>
#include <vector>

namespace blurhash {
struct Image
{
        size_t width, height;
        std::vector<unsigned char> image; // pixels rgb
};

// Decode a blurhash to an image with size width*height
Image
decode(std::string_view blurhash, size_t width, size_t height, size_t bytesPerPixel = 3) noexcept;

// Encode an image of rgb pixels (without padding) with size width*height into a blurhash with x*y
// components
std::string
encode(unsigned char *image, size_t width, size_t height, int x, int y);
}
