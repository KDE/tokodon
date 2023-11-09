// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QByteArray>

#include <openssl/ec.h>

/// Generates a ECDH keypair using the prime256v1 curve.
EC_KEY *generateECDHKeypair();

/// Returns the private key of a ECDH keypair.
QByteArray exportPrivateKey(const EC_KEY *key);

/// Returns the public key of a ECDH keypair.
QByteArray exportPublicKey(const EC_KEY *key);