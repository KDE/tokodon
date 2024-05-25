// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQuick.Effects
import org.kde.kirigami 2 as Kirigami

import "../Components"

// Wrapper component used for the attachment grid
Item {
    id: root

    // The GridLayout holding the media attachments
    required property var gridLayout

    // Whether the media should attempt to limit the aspect ratio to 16:9
    required property bool shouldKeepAspectRatio

    // The width of the AttachmentGrid
    required property real rootWidth

    // The index of this attachment
    required property int index

    // The total number of media attachments
    required property int count

    // Source image dimensions
    required property int sourceWidth
    required property int sourceHeight

    // Chip control
    property bool showGifChip: false
    property bool showVideoChip: false

    property alias containsMouse: mouseArea.containsMouse

    readonly property real aspectRatio: root.sourceHeight / Math.max(root.sourceWidth, 1)
    readonly property real mediaRatio: 9.0 / 16.0

    // If there is three attachments, the first one is bigger than the other two.
    readonly property bool isSpecialAttachment: index == 0 && count == 3

    readonly property int heightDivisor: (isSpecialAttachment || count < 3) ? 1 : 2

    signal clicked()
    signal contextMenuRequested()

    Layout.fillWidth: true
    Layout.fillHeight: !shouldKeepAspectRatio
    Layout.rowSpan: isSpecialAttachment ? 2 : 1

    readonly property real extraSpacing: isSpecialAttachment ? gridLayout.rowSpacing : 0

    Layout.preferredHeight: shouldKeepAspectRatio ? Math.ceil(rootWidth * aspectRatio) : Math.ceil(rootWidth * mediaRatio / heightDivisor) + extraSpacing

    Keys.onSpacePressed: event => clicked()

    activeFocusOnTab: true

    layer.enabled: true
    layer.effect: RoundedEffect {}

    // TODO: port to TapHandler/HoverHandler?
    MouseArea {
        id: mouseArea

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton

        onClicked: root.clicked()
    }

    TapHandler {
        acceptedButtons: Qt.RightButton
        acceptedDevices: PointerDevice.Mouse | PointerDevice.Stylus | PointerDevice.TouchPad
        exclusiveSignals: TapHandler.SingleTap | TapHandler.DoubleTap
        onTapped: root.contextMenuRequested()
    }

    TapHandler {
        acceptedButtons: Qt.LeftButton
        acceptedDevices: PointerDevice.TouchScreen
        exclusiveSignals: TapHandler.SingleTap | TapHandler.DoubleTap
        onLongPressed: root.contextMenuRequested()
    }

    RowLayout {
        spacing: Kirigami.Units.mediumSpacing
        z: 20

        anchors {
            top: parent.top
            topMargin: Kirigami.Units.smallSpacing
            right: parent.right
            rightMargin: Kirigami.Units.smallSpacing
        }

        Kirigami.Chip {
            checked: false
            checkable: false
            text: i18nc("Attachment has alt-text, Short for alt-text", "Alt")
            closable: false
            enabled: false
            visible: modelData.caption.length !== 0
            hoverEnabled: true

            QQC2.ToolTip.text: i18n("Text description available")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
        }

        Kirigami.Chip {
            checked: false
            checkable: false
            text: i18n("GIF")
            closable: false
            enabled: false
            visible: root.showGifChip
        }

        Kirigami.Chip {
            checked: false
            checkable: false
            text: i18n("Video")
            closable: false
            enabled: false
            visible: root.showVideoChip
        }
    }

    Rectangle {
        anchors.fill: parent

        z: 3

        visible: root.activeFocus
        radius: Kirigami.Units.mediumSpacing
        color: "transparent"

        border {
            width: 1
            color: Kirigami.Theme.focusColor
        }
    }
}