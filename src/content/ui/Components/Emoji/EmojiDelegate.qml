// SPDX-FileCopyrightText: 2022 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami

QQC2.ItemDelegate {
    id: emojiDelegate

    property string name
    property string emoji

    property bool showTones: false
    property bool isImage: false

    leftInset: Kirigami.Units.smallSpacing
    topInset: Kirigami.Units.smallSpacing
    rightInset: Kirigami.Units.smallSpacing
    bottomInset: Kirigami.Units.smallSpacing

    QQC2.ToolTip.text: emojiDelegate.name
    QQC2.ToolTip.visible: hovered && emojiDelegate.name !== ""
    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

    contentItem: Item {
        Loader {
            anchors.fill: parent

            active: !emojiDelegate.isImage
            sourceComponent: Kirigami.Heading {
                text: emojiDelegate.emoji
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.family: "emoji"

                Kirigami.Icon {
                    width: Kirigami.Units.gridUnit * 0.5
                    height: Kirigami.Units.gridUnit * 0.5
                    source: "arrow-down"
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    visible: emojiDelegate.showTones
                }
            }
        }

        Loader {
            anchors.fill: parent

            active: emojiDelegate.isImage
            sourceComponent: Image {
                source: visible ? emojiDelegate.emoji : ""
                fillMode: Image.PreserveAspectFit
                cache: true
            }
        }
    }

    background: Rectangle {
        color: emojiDelegate.checked ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor
        radius: Kirigami.Units.smallSpacing

        Rectangle {
            radius: Kirigami.Units.smallSpacing
            anchors.fill: parent
            color: Kirigami.Theme.highlightColor
            opacity: emojiDelegate.hovered && !emojiDelegate.pressed ? 0.2 : 0
        }
    }
}
