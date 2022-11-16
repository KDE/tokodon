// SPDX-FileCopyrightText: 2019 Black Hat <bhat@encom.eu.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.15 as Kirigami

ApplicationWindow {
    id: root

    required property var model
    property alias currentIndex: view.currentIndex

    flags: Qt.FramelessWindowHint | Qt.WA_TranslucentBackground
    visibility: Qt.WindowFullScreen

    title: i18n("Image View")

    Shortcut {
        sequence: "Escape"
        onActivated: root.destroy()
    }

    color: Kirigami.Theme.backgroundColor

    background: AbstractButton {
        onClicked: root.destroy()
    }

    ListView {
        id: view
        anchors.fill: parent
        snapMode: ListView.SnapOneItem 
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightMoveDuration: 0
        focus: true
        keyNavigationEnabled: true
        keyNavigationWraps: true
        model: root.model
        orientation: ListView.Horizontal
        delegate: Item {
            width: ListView.view.width
            height: ListView.view.height
            Image {
                id: image
                anchors.centerIn: parent

                width: Math.min(sourceSize.width, root.width)
                height: Math.min(sourceSize.height, root.height - description.height - Kirigami.Units.gridUnit)
                mipmap: true

                cache: false
                fillMode: Image.PreserveAspectFit
                source: modelData.url

                BusyIndicator {
                    visible: image.status !== Image.Ready
                    anchors.centerIn: parent
                    running: visible
                }
            }

            Label {
                id: description
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: Kirigami.Units.gridUnit
                width: Math.min(root.width, implicitWidth)
                text: modelData.description
            }
        }

        RoundButton {
            anchors {
                left: parent.left
                leftMargin: Kirigami.Units.largeSpacing
                verticalCenter: parent.verticalCenter
            }
            width: Kirigami.Units.gridUnit * 2
            height: width
            icon.name: "arrow-left"
            visible: !Kirigami.Settings.isMobile && view.currentIndex > 0
            Keys.forwardTo: view
            Accessible.name: i18n("Previous image")
            onClicked: view.currentIndex -= 1
        }

        RoundButton {
            anchors {
                right: parent.right
                rightMargin: Kirigami.Units.largeSpacing
                verticalCenter: parent.verticalCenter
            }
            width: Kirigami.Units.gridUnit * 2
            height: width
            icon.name: "arrow-right"
            visible: !Kirigami.Settings.isMobile && view.currentIndex < view.count - 1
            Keys.forwardTo: view
            Accessible.name: i18n("Next image")
            onClicked: view.currentIndex += 1
        }

    }

    Button {
        anchors.top: parent.top
        anchors.right: parent.right

        text: i18n("Close")
        icon.name: "dialog-close"
        display: AbstractButton.IconOnly

        width: Kirigami.Units.gridUnit * 2
        height: Kirigami.Units.gridUnit * 2

        onClicked: root.destroy()
    }
}
