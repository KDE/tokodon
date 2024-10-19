// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts

Kirigami.FlexColumn {
    id: flexColumn

    default property alias content: inner.children

    signal tapped

    spacing: Kirigami.Units.largeSpacing

    padding: 0
    maximumWidth: Kirigami.Units.gridUnit * 40

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true

        Layout.preferredWidth: layout.implicitWidth
        Layout.preferredHeight: layout.implicitHeight

        TapHandler {
            onTapped: flexColumn.tapped()
        }

        RowLayout {
            id: layout

            anchors.fill: parent

            spacing: 0

            Item {
                id: threadSpace

                visible: isThreadReply && isReply

                Layout.preferredWidth: visible ? root.threadMargin : 0
                Layout.fillHeight: true

                Kirigami.Separator {
                    id: threadSeparator

                    readonly property bool shouldDrawFullLine: !root.isLastThreadReply
                    readonly property real avatarOffset: 30

                    anchors {
                        top: parent.top
                        topMargin: -root.topPadding
                        bottom: shouldDrawFullLine ? parent.bottom : undefined
                        bottomMargin: shouldDrawFullLine ? -root.bottomPadding : 0
                        horizontalCenter: threadSpace.horizontalCenter
                    }

                    height: shouldDrawFullLine ? threadSpace.height : threadSeparator.avatarOffset

                    Kirigami.Separator {
                        anchors {
                            top: parent.top
                            topMargin: threadSeparator.avatarOffset
                            left: parent.left
                        }

                        width: threadSeparator.avatarOffset
                    }
                }
            }

            ColumnLayout {
                id: inner

                spacing: Kirigami.Units.largeSpacing
            }
        }
    }
}