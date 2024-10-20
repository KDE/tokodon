// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import org.kde.tokodon

RowLayout {
    id: root

    readonly property bool isBoost: type === Notification.Repeat
    readonly property bool isFavorite: type === Notification.Favorite

    required property var type
    required property var notificationActorIdentity
    required property var numInGroup

    spacing: Kirigami.Units.smallSpacing

    Layout.fillWidth: true

    Kirigami.Icon {
        source: {
            if (root.isBoost) {
                return "tokodon-post-boost"
            } else if (root.isFavorite) {
                return "favorite"
            }

            return ''
        }

        isMask: true

        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
        Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
    }

    QQC2.AbstractButton {
        contentItem: RowLayout {
            Repeater {
                model: root.notificationActorIdentity

                KirigamiComponents.AvatarButton {
                    required property var modelData

                    implicitHeight: Math.round(Kirigami.Units.gridUnit * 1.5)
                    implicitWidth: implicitHeight

                    name: modelData.displayName
                    source: modelData.avatarUrl
                    cache: true

                    onClicked: Navigation.openAccount(modelData.id)

                    QQC2.ToolTip.text: i18n("View %1's Profile", modelData.displayName)
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                }
            }

            QQC2.Label {
                text: {
                    if (root.isFavorite) {
                        return i18n("%1 users favorited your post", root.numInGroup);
                    } else if (root.isBoost) {
                        return i18n("%1 users boosted your post", root.numInGroup);
                    }

                    return '';
                }
                textFormat: Text.RichText
                font.bold: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
            }
        }
    }
}

