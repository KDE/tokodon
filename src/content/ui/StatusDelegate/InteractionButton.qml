// SPDX-FileCopyrightText: 2022 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Controls 2.15 as QQC2

QQC2.AbstractButton {
    id: control

    property string iconSource
    property bool interacted
    property color interactionColor

    hoverEnabled: true

    implicitWidth: icon.width + label.contentWidth + Kirigami.Units.smallSpacing
    implicitHeight: icon.height + Kirigami.Units.smallSpacing

    Kirigami.Icon {
        id: icon

        width: Kirigami.Units.iconSizes.smallMedium
        height: Kirigami.Units.iconSizes.smallMedium

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

        source: parent.iconSource

        isMask: true
        color: control.hovered ? Kirigami.Theme.focusColor : (interacted ? control.interactionColor : Kirigami.Theme.textColor)
    }

    QQC2.Label {
        id: label

        anchors.left: icon.right
        anchors.leftMargin: Kirigami.Units.smallSpacing
        anchors.verticalCenter: parent.verticalCenter

        text: control.text
    }
}