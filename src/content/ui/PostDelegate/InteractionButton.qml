// SPDX-FileCopyrightText: 2022 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2

/**
 * @brief A specialized button used for the "Favorite", "Boost", etc buttons on a status
 */
QQC2.ToolButton {
    id: control

    required property string iconName
    required property string tooltip

    property string interactedIconName
    property bool interactable: true
    property bool interacted: false
    property color interactionColor

    display: QQC2.AbstractButton.IconOnly
    activeFocusOnTab: interactable

    QQC2.ToolTip.text: control.tooltip
    QQC2.ToolTip.visible: hovered && QQC2.ToolTip.text !== ""
    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

    Accessible.name: tooltip

    contentItem: RowLayout {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Icon {
            id: icon

            Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
            Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            source: control.interacted ? control.interactedIconName : control.iconName

            isMask: true
            color: (interactable && parent.activeFocus) ? Kirigami.Theme.focusColor : (control.interacted ? control.interactionColor : Kirigami.Theme.textColor)
        }

        QQC2.Label {
            id: label

            text: control.text
            verticalAlignment: Text.AlignVCenter
            visible: control.text
            color: Kirigami.Theme.disabledTextColor

            Layout.rightMargin: Kirigami.Units.smallSpacing
        }
    }
}
