// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard

import org.kde.tokodon

MastoPage {
    id: root

    title: i18nc("@title:window", "Welcome")

    Image {
        source: "qrc:/org.kde.tokodon.svg"

        fillMode: Image.PreserveAspectFit

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: Kirigami.Units.largeSpacing * 3
    }

    FormCard.FormHeader {
        title: i18n("Welcome to Tokodon")
    }

    FormCard.FormCard {
        FormCard.FormButtonDelegate {
            id: learnMoreButton
            text: i18n("Learn More")
            icon.name: "globe"
            onClicked: Qt.openUrlExternally("https://joinmastodon.org/")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: registerButton
            text: i18n("Register")
            icon.name: "list-add-user"
            onClicked: {
                if (Kirigami.Settings.isMobile) {
                    QQC2.ApplicationWindow.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "ServersPage"))
                } else {
                    QQC2.ApplicationWindow.window.pageStack.push(Qt.createComponent("org.kde.tokodon", "ServersPage"))
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: loginButton
            text: i18n("Login")
            icon.name: "user"
            onClicked: {
                if (Kirigami.Settings.isMobile) {
                    QQC2.ApplicationWindow.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "LoginPage"))
                } else {
                    QQC2.ApplicationWindow.window.pageStack.push(Qt.createComponent("org.kde.tokodon", "LoginPage"))
                }
            }
        }
    }
}
