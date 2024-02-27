// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import org.kde.tokodon
import org.kde.tokodon.private

// The label that sits above posts, e.g. "FooBar replied to" or "BarFoo boosted"
RowLayout {
    id: root

    readonly property var identity: {
        if (replyAuthorIdentity) {
            return replyAuthorIdentity;
        } else if (boostAuthorIdentity) {
            return boostAuthorIdentity;
        }
    }

    required property bool isBoosted
    required property bool isReply
    required property var type
    required property var boostAuthorIdentity
    required property var replyAuthorIdentity

    Layout.fillWidth: true

    Kirigami.Icon {
        source: {
            if (root.isBoosted) {
                return "tokodon-post-boost"
            } else if (root.isReply) {
                return "tokodon-post-reply"
            }

            return ''
        }

        isMask: true
        color: Kirigami.Theme.disabledTextColor

        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
        Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
    }

    QQC2.AbstractButton {
        contentItem: RowLayout {
            KirigamiComponents.AvatarButton {
                implicitHeight: Math.round(Kirigami.Units.gridUnit * 1.5)
                implicitWidth: implicitHeight

                name: root.identity ? root.identity.displayName : ''
                source: root.identity ? root.identity.avatarUrl : ''
                cache: true

                onClicked: Navigation.openAccount(root.identity.id)

                QQC2.ToolTip.text: i18n("View Profile")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
            QQC2.Label {
                text: {
                    if (root.isBoosted) {
                        return root.identity ? i18n("%1 boosted", root.identity.displayNameHtml) : '';
                    } else if (root.isReply) {
                        return root.identity ? i18n("In reply to %1", root.identity.displayNameHtml) : '';
                    }
                }
                color: Kirigami.Theme.disabledTextColor
                font: Config.defaultFont

                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
            }
        }
    }
}