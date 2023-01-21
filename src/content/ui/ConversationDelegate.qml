// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import org.kde.kirigami 2.20 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

Kirigami.AbstractListItem {
    id: root

    required property int index
    required property string id
    required property string content
    required property bool unread
    required property var authorIdentity
    required property string relativeTime
    required property int conversationsCount
    required property string conversationId

    readonly property bool showSeparator: root.index !== conversationsCount - 1

    signal markAsRead(conversationId: string)

    leftPadding: Kirigami.Units.gridUnit
    rightPadding: Kirigami.Units.gridUnit
    topPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing

    onClicked: {
        Navigation.openThread(root.id)
        if (root.unread) {
            root.markAsRead(root.conversationId);
        }
    }

    hoverEnabled: true

    background: Rectangle {
        color: Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, root.showSelected ? 0.5 : hoverHandler.hovered ? 0.2 : 0)

        // indicator rectangle
        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 1

            width: 4
            visible: root.unread
            color: Kirigami.Theme.highlightColor
        }

        HoverHandler {
            id: hoverHandler
            // disable hover input on mobile because touchscreens trigger hover feedback and do not "unhover" in Qt
            enabled: !Kirigami.Settings.isMobile
        }

        Kirigami.Separator {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: root.leftPadding
            anchors.rightMargin: root.rightPadding
            visible: root.showSeparator && !root.showSelected
            opacity: 0.5
        }
    }

    Item {
        id: item
        implicitHeight: rowLayout.implicitHeight

        RowLayout {
            id: rowLayout
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            Kirigami.Avatar {
                name: root.authorIdentity.displayName
                source: root.authorIdentity.avatarUrl
                Layout.rightMargin: Kirigami.Units.largeSpacing
                sourceSize.width: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                sourceSize.height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
                Layout.preferredHeight: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                RowLayout {
                    Layout.fillWidth: true
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: root.authorIdentity.displayNameHtml
                        elide: Text.ElideRight
                        font.weight: root.unread ? Font.Bold : Font.Normal
                    }
                    QQC2.Label {
                        text: root.relativeTime
                        color: Kirigami.Theme.disabledTextColor
                    }
                }
                QQC2.Label {
                    color: Kirigami.Theme.disabledTextColor
                    Layout.fillWidth: true
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    text: root.content
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onClicked: {
                if (mouse.button === Qt.RightButton) {
                    root.contextMenuRequested();
                } else if (mouse.button === Qt.LeftButton) {
                    root.clicked();
                }
            }
            onPressAndHold: root.contextMenuRequested();
        }
    }
}
