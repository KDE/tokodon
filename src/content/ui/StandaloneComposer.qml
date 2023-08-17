// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import org.kde.kmasto 1.0

import "./StatusComposer"
import "./StatusDelegate"

Kirigami.ApplicationWindow {
    id: appwindow

    minimumWidth: Kirigami.Units.gridUnit * 15
    minimumHeight: Kirigami.Units.gridUnit * 20

    Connections {
        target: AccountManager

        function onAccountsReady() {
            if (!AccountManager.hasAccounts) {
                missingAccountMessage.visible = true;
            }
        }
    }

    Connections {
        target: Controller

        function onOpenComposer(text) {
            pageStack.push(Qt.resolvedUrl("./StatusComposer/StatusComposer.qml"), {
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
