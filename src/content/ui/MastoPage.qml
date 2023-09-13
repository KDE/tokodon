// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami

Kirigami.ScrollablePage {
    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Image {
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            source: "qrc:/content/elephant.svg"
        }
    }
}
