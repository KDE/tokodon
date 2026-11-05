// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma Singleton
pragma ComponentBehavior: Bound

import QtQml

/**
 * Typed handle to the main application window.
 */
QtObject {
    property Main instance: null
}
