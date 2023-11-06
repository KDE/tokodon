// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami

Flow {
    id: root

    required property var standaloneTags

    spacing: Kirigami.Units.largeSpacing
    visible: root.standaloneTags.length !== 0

    Repeater {
        model: root.standaloneTags

        Kirigami.Chip {
            required property string modelData

            closable: false
            checkable: false

            text: "#" + modelData

            onClicked: Navigation.openTag(modelData)
        }
    }
}