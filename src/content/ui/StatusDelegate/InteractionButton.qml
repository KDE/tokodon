// SPDX-FileCopyrightText: 2022 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.4 as Kirigami
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

    implicitWidth: icon.width + (control.text.length > 0 ? label.contentWidth + Kirigami.Units.largeSpacing * 2 : 0) + rightPadding
    implicitHeight: icon.height

    rightPadding: Kirigami.Units.largeSpacing * 2

    QQC2.ToolTip.text: control.tooltip
    QQC2.ToolTip.visible: hovered
    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

    Accessible.name: tooltip

    Kirigami.Icon {
        id: icon

        width: Kirigami.Units.iconSizes.smallMedium
        height: Kirigami.Units.iconSizes.smallMedium

        source: control.interacted ? control.interactedIconName : control.iconName

        isMask: true
        color: (interactable && (control.hovered || parent.activeFocus)) ? Kirigami.Theme.focusColor : (control.interacted ? control.interactionColor : Kirigami.Theme.textColor)
    }

    QQC2.Label {
        id: label

        anchors {
            left: icon.right
            leftMargin: Kirigami.Units.smallSpacing

            top: parent.top
            bottom: parent.bottom
        }

        text: control.text
        color: control.textColor
        verticalAlignment: Text.AlignVCenter
    }
}
