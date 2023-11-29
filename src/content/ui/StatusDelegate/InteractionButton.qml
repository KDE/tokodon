// SPDX-FileCopyrightText: 2022 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2

// A specialized button used for the "Favorite", "Boost", and etc buttons on a status
QQC2.AbstractButton {
    id: control

    required property string iconName
    property string interactedIconName

    property bool interactable: true
    property bool interacted: false
    property color interactionColor

    required property string tooltip

    hoverEnabled: true
    activeFocusOnTab: interactable

    QQC2.ToolTip.text: control.tooltip
    QQC2.ToolTip.visible: hovered && QQC2.ToolTip.text !== ""
    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

    Accessible.name: tooltip

    topPadding: Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.smallSpacing
    leftPadding: Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.smallSpacing

    contentItem: RowLayout {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Icon {
            id: icon

            Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
            Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            source: control.interacted ? control.interactedIconName : control.iconName

            isMask: true
            color: (interactable && (control.hovered || parent.activeFocus)) ? Kirigami.Theme.focusColor : (control.interacted ? control.interactionColor : Kirigami.Theme.textColor)
        }

        QQC2.Label {
            id: label

            text: control.text
            verticalAlignment: Text.AlignVCenter
            visible: control.text
            color: Kirigami.Theme.disabledTextColor
        }
    }

    background: Rectangle {
        color: control.hovered && control.interactable ? Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.1) : "transparent"
        radius: Kirigami.Units.mediumSpacing

        Rectangle {
            visible: control.visualFocus
            radius: parent.radius
            color: "transparent"
            anchors {
                fill: parent
                margins: -2
            }
            border {
                width: 2
                color: Kirigami.Theme.focusColor
            }
        }
    }
}
