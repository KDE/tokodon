// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Layouts

Kirigami.FlexColumn {
    id: flexColumn

    required property bool isThreadReply
    required property bool isReply
    required property real threadMargin
    required property bool isLastThreadReply

    default property alias content: inner.children

    spacing: Kirigami.Units.largeSpacing

    padding: 0
    maximumWidth: Kirigami.Units.gridUnit * 40

    RowLayout {
        spacing: 0

        Item {
            id: threadSpace

            visible: flexColumn.isThreadReply && flexColumn.isReply

            Layout.preferredWidth: visible ? flexColumn.threadMargin : 0
            Layout.fillHeight: true

            Kirigami.Separator {
                id: threadSeparator

                readonly property bool shouldDrawFullLine: !flexColumn.isLastThreadReply
                readonly property real avatarOffset: 30

                anchors {
                    top: parent.top
                    topMargin: -Kirigami.Units.largeSpacing
                    bottom: shouldDrawFullLine ? parent.bottom : undefined
                    bottomMargin: shouldDrawFullLine ? -Kirigami.Units.largeSpacing : 0
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
