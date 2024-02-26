// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "utils/ecdh.h"

#include <openssl/obj_mac.h>

constexpr int PRIVATE_KEY_LENGTH = 32;
constexpr int PUBLIC_KEY_LENGTH = 65;

EC_KEY *generateECDHKeypair()
{
    EC_KEY *key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!key) {
        return nullptr;
    }

    if (EC_KEY_generate_key(key) != 1) {
        EC_KEY_free(key);
        return nullptr;
    }
    return key;
}

QByteArray exportPrivateKey(const EC_KEY *key)
{
    uint8_t rawPrivKey[PRIVATE_KEY_LENGTH];
    if (!EC_KEY_priv2oct(key, rawPrivKey, PRIVATE_KEY_LENGTH)) {
        return {};
    }

    return {reinterpret_cast<const char *>(rawPrivKey), PRIVATE_KEY_LENGTH};
}

QByteArray exportPublicKey(const EC_KEY *key)
{
    uint8_t rawPubKey[PUBLIC_KEY_LENGTH];
    if (!EC_POINT_point2oct(EC_KEY_get0_group(key), EC_KEY_get0_public_key(key), POINT_CONVERSION_UNCOMPRESSED, rawPubKey, PUBLIC_KEY_LENGTH, nullptr)) {
        return {};
    }

    return {reinterpret_cast<const char *>(rawPubKey), PUBLIC_KEY_LENGTH};
}