// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami

Kirigami.ShadowedRectangle {
    color: Kirigami.Theme.backgroundColor
    radius: Kirigami.Units.mediumSpacing
    shadow {
        size: Kirigami.Units.largeSpacing
        color: Qt.rgba(0.0, 0.0, 0.0, 0.3)
        yOffset: 2
    }
    border {
        color: Kirigami.ColorUtils.tintWithAlpha(color, Kirigami.Theme.textColor, 0.15)
        width: 1
    }
}