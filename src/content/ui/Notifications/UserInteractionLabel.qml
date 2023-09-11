// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.components 1.0 as KirigamiComponents

import org.kde.kmasto 1.0

RowLayout {
    id: root

    readonly property bool isBoost: type === Notification.Repeat
    readonly property bool isReply: type === Notification.Reply || type === Notification.Mention
    readonly property bool isFavorite: type === Notification.Favorite
    readonly property bool isPoll: type === Notification.Poll
    readonly property bool isUpdate: type === Notification.Update

    required property var type
    required property var notificationActorIdentity

    Layout.fillWidth: true

    Kirigami.Icon {
        source: {
            if (root.isBoost) {
                return "post-boost"
            } else if (root.isReply) {
                return "post-reply"
            } else if (root.isFavorite) {
                return "favorite"
            } else if (root.isUpdate) {
                return "cell_edit"
            } else if (root.isPoll) {
                return "office-chart-bar"
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
            KirigamiComponents.AvatarButton {
                implicitHeight: Math.round(Kirigami.Units.gridUnit * 1.5)
                implicitWidth: implicitHeight

                name: root.notificationActorIdentity ? root.notificationActorIdentity.displayName : ''
                source: root.notificationActorIdentity ? root.notificationActorIdentity.avatarUrl : ''
                cache: true
                visible: root.isFavorite || root.isBoost

                onClicked: Navigation.openAccount(root.notificationActorIdentity.id)

                QQC2.ToolTip.text: i18n("View Profile")
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
                        return i18n("%1 replied to your post", root.notificationActorIdentity.displayNameHtml);
                    } else if (root.isFavorite) {
                        return i18n("%1 favorited your post", root.notificationActorIdentity.displayNameHtml);
                    } else if (root.isPoll) {
                        return i18n("A poll has ended");
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