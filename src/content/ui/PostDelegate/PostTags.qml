// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon

/**
 * @brief The line of tags that are considered "standalone" (e.g. are not embedded in a paragraph)
 *
 * This usually sits below the media attachment
 */
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