// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon

import '..'
import '../PostDelegate'

Kirigami.AbstractCard {
    id: root

    required property var userIdentity

    showClickFeedback: true
    onClicked: Navigation.openAccount(root.userIdentity.id)

    contentItem: Item {
        implicitHeight: userInfo.implicitHeight
        anchors {
            leftMargin: Kirigami.Units.mediumSpacing
            rightMargin: Kirigami.Units.mediumSpacing
            topMargin: Kirigami.Units.mediumSpacing
        }

        ColumnLayout {
            id: userInfo

            spacing: Kirigami.Units.smallSpacing

            InlineIdentityInfo {
                identity: root.userIdentity
                Layout.bottomMargin: Kirigami.Units.smallSpacing
            }

            QQC2.TextArea {
                id: bio
                visible: root.userIdentity.bio && root.userIdentity.bio.length > 0
                font: Config.defaultFont
                width: root.width - Kirigami.Units.largeSpacing * 2
                text:  "<style>
                            a {
                            color: " + Kirigami.Theme.linkColor + ";
                            text-decoration: none;
                            }
                            </style>" + root.userIdentity.bio
                textFormat: TextEdit.RichText
                readOnly: true
                background: null
                wrapMode: TextEdit.Wrap
                selectByMouse: false
                color: Kirigami.Theme.textColor
                hoverEnabled: false

                // So this text area doesn't eat the mouse events
                onReleased: root.clicked()
            }
        }
    }
}
