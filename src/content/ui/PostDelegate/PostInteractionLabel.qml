// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import org.kde.tokodon

// The label that sits above posts, e.g. "FooBar replied to" or "BarFoo boosted"
RowLayout {
    id: root

    spacing: Kirigami.Units.smallSpacing

    readonly property var identity: {
        if (boostAuthorIdentity) {
            return boostAuthorIdentity;
        } else if (replyAuthorIdentity) {
            return replyAuthorIdentity;
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
                return "boost"
            } else if (root.isReply) {
                return "view-conversation-balloon-symbolic"
            }

            return ''
        }

        isMask: true
        color: Kirigami.Theme.disabledTextColor

        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
        Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
    }

    QQC2.Label {
        id: interactionLabel

        text: {
            if (!root.identity) {
                return i18nc("@info Loading user that started this interaction", "Loading…");
            }

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

    TapHandler {
        acceptedButtons: Qt.LeftButton
        onTapped: Navigation.openAccount(root.identity.id)
    }

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }
}
