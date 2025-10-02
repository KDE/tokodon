// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2

import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon

/**
 * @brief The line of tags that are considered "standalone" (e.g. are not embedded in a paragraph)
 *
 * This usually sits below the media attachment
 */
QQC2.Control {
    id: root

    required property list<string> standaloneTags

    readonly property int conciseTagCount: 3
    readonly property int hiddenTagCount: Math.max(standaloneTags.length - conciseTagCount, 0)
    property bool expanded: false

    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0
    topPadding: 0

    visible: root.standaloneTags.length !== 0

    background: null

    contentItem: Flow {
        spacing: Kirigami.Units.smallSpacing

        Repeater {
            model: root.expanded ? root.standaloneTags : root.standaloneTags.slice(0, root.conciseTagCount);

            Kirigami.Chip {
                required property string modelData

                closable: false
                checkable: false

                text: "#" + modelData

                onClicked: Navigation.openTag(modelData)

                Accessible.name: i18nc("@info", "Tag")
                Accessible.description: modelData
            }
        }

        Kirigami.Chip {
            text: i18nc("@action:button expand the number of shown tags", "…and %1 more", root.hiddenTagCount)
            visible: !root.expanded && root.hiddenTagCount > 0
            closable: false
            checkable: false

            onClicked: root.expanded = true
        }
    }
}
