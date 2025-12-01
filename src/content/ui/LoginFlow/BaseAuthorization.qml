// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kquickcontrolsaddons as KQuickControlsAddons

import org.kde.tokodon

/**
 * Base page for all authorization pages.
 */
Kirigami.ScrollablePage {
    id: root

    default property alias children: layout.data

    required property var account
    required property var loginPage
    property bool needsAuthLink: true

    title: i18nc("@title:window", "Authorization")

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    data: Connections {
        target: Controller

        function onReceivedAuthCode(authCode) {
            root.setAuthCode(authCode);
        }
    }

    function setAuthCode(authCode) {
        // We need to call this before setToken, so when the identity is loaded the manager can write it to the settings.
        AccountManager.addAccount(root.account);
        account.authenticated.connect(() => {
            AccountManager.selectedAccount = root.account;
            root.Window.window.pageStack.layers.clear();
            root.Window.window.pageStack.replace(mainTimeline, {
                name: "home"
            });
            if (root.Window.window !== applicationWindow()) {
                root.Window.window.close();
            }
        });
        account.setToken(authCode);
    }

    KQuickControlsAddons.Clipboard {
        id: clipboard
    }

    ColumnLayout {
        id: layout

        anchors.fill: root.needsAuthLink ? null : parent

        spacing: Kirigami.Units.largeSpacing

        FormCard.FormCard {
            visible: root.needsAuthLink

            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Layout.preferredHeight: implicitHeight

            FormCard.FormTextDelegate {
                Layout.fillWidth: true

                text: i18n("To continue, you must authorize Tokodon to access your account.")
            }

            FormCard.FormDelegateSeparator {
                above: openLink
            }

            FormCard.FormButtonDelegate {
                id: openLink
                text: i18n("Open Authorization Page")
                icon.name: "document-open"
                onClicked: Qt.openUrlExternally(account.authorizeUrl)
            }

            FormCard.FormDelegateSeparator {
                above: openLink
                below: copyLink
            }

            FormCard.FormButtonDelegate {
                id: copyLink
                text: i18n("Copy Link to Authorization Page")
                icon.name: "edit-copy"
                onClicked: {
                    clipboard.content = account.authorizeUrl;
                    applicationWindow().showPassiveNotification(i18n("Link copied."));
                }
            }
        }
    }
}
