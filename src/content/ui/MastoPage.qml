// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami

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
