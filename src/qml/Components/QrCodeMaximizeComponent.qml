// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.kirigamiaddons.labs.components as KirigamiComponents
import org.kde.kirigami as Kirigami
import org.kde.prison as Prison

KirigamiComponents.AbstractMaximizeComponent {
    id: root

    required property string url

    content: Item {
        Prison.Barcode {
            id: barcode

            content: root.url
            barcodeType: Prison.Barcode.QRCode
            height: Math.min(parent.height, Kirigami.Units.gridUnit * 20)
            width: height

            anchors.centerIn: parent
        }
    }
}
