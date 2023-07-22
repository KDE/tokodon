// SPDX-FileCopyrightText: 2022 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import QtQuick.Controls 2.15 as QQC2

QQC2.AbstractButton {
    id: control

    required property string iconName
    property string interactedIconName

    property bool interactable: true
    property bool interacted: false
    property color interactionColor

    property color textColor: Kirigami.Theme.textColor

    required property string tooltip

    hoverEnabled: true
    activeFocusOnTab: interactable

    QQC2.ToolTip.text: control.tooltip
    QQC2.ToolTip.visible: hovered
    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

    Accessible.name: tooltip

    bottomPadding: Kirigami.Units.smallSpacing
    topPadding: Kirigami.Units.smallSpacing

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
            color: control.textColor
            verticalAlignment: Text.AlignVCenter
            visible: control.text
        }
    }

    background: Rectangle {
        color: control.hovered && control.interactable ? Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.1) : "transparent"
        radius: Kirigami.Units.smallSpacing

        Rectangle {
            visible: control.visualFocus
            radius: Kirigami.Units.smallSpacing
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
