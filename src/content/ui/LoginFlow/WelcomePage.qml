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

    property alias showSettingsButton: settingsCard.visible

    header: Kirigami.Separator {
        width: root.width
    }

    property TokodonConfigurationView settingsWindow: TokodonConfigurationView {}

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

                Layout.preferredWidth: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
                Layout.preferredHeight: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
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
                text: i18nc("@info", "Browse the Fediverse and connect with people on Mastodon (and other compatible software).")
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.maximumWidth: Kirigami.Units.gridUnit * 20
            }

            FormCard.FormCard {
                Layout.topMargin: Kirigami.Units.largeSpacing

                maximumWidth: Kirigami.Units.gridUnit * 20

                FormCard.FormButtonDelegate {
                    id: registerButton
                    text: i18nc("@action:button Pick a Mastodon server", "Pick a Server")
                    icon.name: "network-server-symbolic"
                    onClicked: Window.window.pageStack.push(Qt.createComponent("org.kde.tokodon", "ServersPage"))
                    focus: true
                }

                FormCard.FormDelegateSeparator {}

                FormCard.FormButtonDelegate {
                    id: loginButton
                    text: i18nc("@action:button Use an existing Mastodon account", "Use an Existing Account")
                    icon.name: "user-symbolic"
                    onClicked: Window.window.pageStack.push(Qt.createComponent("org.kde.tokodon", "LoginPage"))
                }
            }

            FormCard.FormCard {
                id: settingsCard

                Layout.topMargin: Kirigami.Units.largeSpacing

                maximumWidth: Kirigami.Units.gridUnit * 20

                FormCard.FormButtonDelegate {
                    id: settingsButton
                    text: i18nc("@action:button Application settings", "Settings")
                    icon.name: "settings-configure"
                    onClicked: settingsWindow.open()
                }
            }

            QQC2.AbstractButton {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: Kirigami.Units.largeSpacing

                contentItem: ColumnLayout {
                    spacing: Kirigami.Units.smallSpacing

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

                        Layout.preferredWidth: Kirigami.Units.gridUnit * 6
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
                }

                onClicked: Qt.openUrlExternally("https://joinmastodon.org/")

                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }
}
