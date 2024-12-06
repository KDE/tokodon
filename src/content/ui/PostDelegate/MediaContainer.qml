// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami

import "../Components"

// Wrapper component used for the attachment grid
Item {
    id: root

    required property string caption

    // Chip control
    property bool showGifChip: false
    property bool showVideoChip: false

    signal clicked()
    signal contextMenuRequested()

    Keys.onSpacePressed: event => clicked()

    activeFocusOnTab: true

    layer.enabled: true
    layer.effect: RoundedEffect {}

    TapHandler {
        acceptedButtons: Qt.LeftButton
        gesturePolicy: TapHandler.WithinBounds
        onTapped: root.clicked()
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
            visible: root.caption.length !== 0
            hoverEnabled: true

            onClicked: {
                altPopupLoader.active = true;
                altPopupLoader.item.subtitle = root.caption;
                altPopupLoader.item.open();
            }

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
        radius: Kirigami.Units.cornerRadius
        color: "transparent"

        border {
            width: 1
            color: Kirigami.Theme.focusColor
        }
    }

    Loader {
        id: altPopupLoader

        active: false
        sourceComponent: Kirigami.PromptDialog {
            width: 400
            height: 300

            title: i18nc("@title", "Media Description")

            onClosed: altPopupLoader.active = false
        }
    }
}
