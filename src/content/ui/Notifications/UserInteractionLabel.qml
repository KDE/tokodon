// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiComponents

import org.kde.tokodon

RowLayout {
    id: root

    readonly property bool isBoost: type === Notification.Repeat
    readonly property bool isReply: type === Notification.Reply || type === Notification.Mention
    readonly property bool isFavorite: type === Notification.Favorite
    readonly property bool isPoll: type === Notification.Poll
    readonly property bool isUpdate: type === Notification.Update
    readonly property bool isStatus: type === Notification.Status

    required property var type
    required property var notificationActorIdentity

    spacing: Kirigami.Units.smallSpacing

    Layout.fillWidth: true

    Kirigami.Icon {
        source: {
            if (root.isBoost) {
                return "boost"
            } else if (root.isReply || root.isStatus) {
                return "view-conversation-balloon-symbolic"
            } else if (root.isFavorite) {
                return "favorite"
            } else if (root.isUpdate) {
                return "cell_edit"
            } else if (root.isPoll) {
                return "amarok_playcount"
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
            spacing: Kirigami.Units.smallSpacing
            KirigamiComponents.AvatarButton {
                implicitHeight: Math.round(Kirigami.Units.gridUnit * 1.5)
                implicitWidth: implicitHeight

                name: root.notificationActorIdentity ? root.notificationActorIdentity.displayName : ''
                source: root.notificationActorIdentity ? root.notificationActorIdentity.avatarUrl : ''
                cache: true
                visible: root.isFavorite || root.isBoost

                onClicked: Navigation.openAccount(root.notificationActorIdentity.id)

                QQC2.ToolTip.text: i18n("View profile")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
            QQC2.Label {
                text: {
                    if (root.notificationActorIdentity === null) {
                        return '';
                    }

                    if (root.isBoost) {
                        return i18n("%1 boosted your post", root.notificationActorIdentity.displayNameHtml);
                    } else if (root.isReply) {
                        return i18n("%1 mentioned you", root.notificationActorIdentity.displayNameHtml);
                    } else if (root.isFavorite) {
                        return i18n("%1 favorited your post", root.notificationActorIdentity.displayNameHtml);
                    } else if (root.isPoll) {
                        return i18n("A poll has ended");
                    } else if (root.isUpdate) {
                        return i18n("%1 updated their post", root.notificationActorIdentity.displayNameHtml)
                    } else if (root.isStatus) {
                        return i18n("%1 wrote a new post", root.notificationActorIdentity.displayNameHtml)
                    }

                    return '';
                }
                font.bold: true

                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
            }
        }
    }
}
