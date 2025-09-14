// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon

import "./StatusComposer"
import "./PostDelegate"

Kirigami.ApplicationWindow {
    id: root

    minimumWidth: Kirigami.Units.gridUnit * 15
    minimumHeight: Kirigami.Units.gridUnit * 20

    Connections {
        target: AccountManager

        function onAccountsReady(): void {
            if (!AccountManager.hasAccounts) {
                missingAccountMessage.visible = true;
            }
        }
    }

    Connections {
        target: Controller

        function onOpenComposer(text: string): void {
            root.pageStack.push(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
                purpose: StatusComposer.New,
                initialText: text,
                closeApplicationWhenFinished: true,
            });
        }
    }

    Rectangle {
        anchors.fill: parent
        visible: !AccountManager.isReady
        color: Kirigami.Theme.backgroundColor

        Kirigami.LoadingPlaceholder {
            anchors.centerIn: parent
        }
    }

    Kirigami.PlaceholderMessage {
        id: missingAccountMessage

        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4

        text: i18n("No accounts available")
        visible: false
    }
}
