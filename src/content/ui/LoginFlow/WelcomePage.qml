// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Effects

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.tokodon

Kirigami.Page {
    id: root

    title: i18nc("@title:window", "Welcome")
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None

    readonly property real minimumWindowWidth: 800
    readonly property bool wideScreen: root.width > minimumWindowWidth

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor

        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        Image {
            anchors {
                left: parent.left
                bottom: parent.bottom
            }

            source: "qrc:/content/elephant.svg"
            opacity: root.wideScreen ? 1.0 : 0.0

            Behavior on opacity {
                OpacityAnimator {
                    duration: Kirigami.Units.shortDuration
                }
            }

            LayoutMirroring.enabled: false
        }

        ColumnLayout {
            spacing: Kirigami.Units.smallSpacing
            opacity: root.wideScreen ? 1.0 : 0.0

            Behavior on opacity {
                OpacityAnimator {
                    duration: Kirigami.Units.shortDuration
                }
            }

            anchors {
                bottom: parent.bottom
                bottomMargin: root.bottomPadding
                right: parent.right
                rightMargin: root.rightPadding
            }

            QQC2.Label {
                text: i18nc("@info Compatible with Mastodon", "Compatible with")
            }

            Image {
                id: mastodonLogoImage

                source: "qrc:/content/mastodon-logo.svg"
                fillMode: Image.PreserveAspectFit

                Accessible.role: Accessible.Button
                Accessible.name: i18nc("@action:button", "Compatible with Mastodon")
                Accessible.onPressAction: Qt.openUrlExternally("https://joinmastodon.org/")

                Layout.preferredWidth: Kirigami.Units.gridUnit * 12
                Layout.preferredHeight: paintedHeight
                Layout.alignment: Qt.AlignRight

                LayoutMirroring.enabled: false

                layer.enabled: true
                layer.effect: MultiEffect {
                    blurMax: 14
                    shadowEnabled: true
                    shadowColor: "black"
                    source: mastodonLogoImage
                }
            }

            TapHandler {
                onTapped: Qt.openUrlExternally("https://joinmastodon.org/")
            }

            HoverHandler {
                cursorShape: Qt.PointingHandCursor
            }
        }
    }

    contentItem: Item {
        ColumnLayout {
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            spacing: Kirigami.Units.largeSpacing

            Image {
                source: "qrc:/org.kde.tokodon.svg"

                fillMode: Image.PreserveAspectFit

                Layout.fillWidth: true
            }

            Kirigami.Heading {
                text: i18nc("@info", "Tokodon")
                level: 1
                horizontalAlignment: Text.AlignHCenter
                font.bold: true

                Layout.fillWidth: true
            }

            QQC2.Label {
                text: i18nc("@info", "Browse the Fediverse and connect with people on Mastodon (and other compatible software.)")
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap

                Layout.fillWidth: true
            }

            FormCard.FormCard {
                Layout.topMargin: Kirigami.Units.largeSpacing

                maximumWidth: Kirigami.Units.gridUnit * 16

                Kirigami.Theme.colorSet: Kirigami.Theme.Selection
                Kirigami.Theme.inherit: false

                FormCard.FormButtonDelegate {
                    id: registerButton
                    text: i18nc("@action:button Pick a Mastodon server", "Pick a Server")
                    icon.name: "network-server-symbolic"
                    onClicked: Window.window.pageStack.push(Qt.createComponent("org.kde.tokodon", "ServersPage"))
                    focus: true
                }
            }

            FormCard.FormCard {
                maximumWidth: Kirigami.Units.gridUnit * 16

                FormCard.FormButtonDelegate {
                    id: loginButton
                    text: i18nc("@action:button Use an existing Mastodon account", "Use an Existing Account")
                    icon.name: "user-symbolic"
                    onClicked: Window.window.pageStack.push(Qt.createComponent("org.kde.tokodon", "LoginPage"))
                }
            }
        }

        FormCard.FormCard {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            maximumWidth: Kirigami.Units.gridUnit * 16

            FormCard.FormButtonDelegate {
                id: settingsButton
                text: i18nc("@action:button Application settings", "Settings")
                icon.name: "settings-configure"
                onClicked: TokodonConfigurationView.open()
            }
        }
    }
}
