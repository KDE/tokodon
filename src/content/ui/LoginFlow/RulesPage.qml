// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as Components

import org.kde.tokodon

MastoPage {
    id: root

    title: i18nc("@title:window", "Rules")

    required property var account

    data: [
        Connections {
            target: Controller
            function onNetworkErrorOccurred(error) {
                message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your proxy settings.", error)
                message.visible = true;
            }
        }
    ]

    header: Components.Banner {
        id: message
        type: Kirigami.MessageType.Error
        width: parent.width

        showCloseButton: true

        actions: Kirigami.Action {
            text: i18n("Proxy Settings")
            icon.name: "settings-configure"
            onTriggered: pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "NetworkProxyPage"))
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Rules")
    }

    FormCard.FormCard {
        Repeater {
            model: RulesModel {
                account: root.account
            }

            delegate: ColumnLayout {
                id: ruleLayout

                required property int index
                required property string text

                spacing: 0

                FormCard.FormDelegateSeparator {
                    visible: index !== 0
                }

                FormCard.FormTextDelegate {
                    text: ruleLayout.text
                }
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormButtonDelegate {
            id: agreeButton

            text: i18nc("@action:button Agree to server rules", "Agree")
            icon.name: "dialog-ok"

            onClicked: {
                root.Window.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "RegistrationPage"), {
                    account: account,
                });
            }
        }

        FormCard.FormDelegateSeparator {
            above: agreeButton
            below: disagreeButton
        }

        FormCard.FormButtonDelegate {
            id: disagreeButton

            text: i18nc("@action:button Disagree to server rules", "Disagree")
            icon.name: "dialog-cancel"

            onClicked: root.Window.window.pageStack.layers.pop()
        }
    }
}
