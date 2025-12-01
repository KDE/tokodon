// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigamiaddons.formcard as FormCard

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

    header: Kirigami.InlineMessage {
        id: message
        type: Kirigami.MessageType.Error
        width: parent.width
        position: Kirigami.InlineMessage.Position.Header

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
            id: rulesRepeater

            model: RulesModel {
                id: rulesModel

                account: root.account

                onLoadingChanged: {
                    if (!rulesModel.loading) {
                        // Announce the first rule to the screen reader
                        rulesRepeater.itemAt(0).forceFocus();
                    }
                }
            }

            delegate: ColumnLayout {
                id: ruleLayout

                required property int index
                required property string text

                spacing: 0

                function forceFocus(): void {
                    ruleTextDelegate.forceActiveFocus();
                }

                FormCard.FormDelegateSeparator {
                    visible: index !== 0
                }

                FormCard.FormTextDelegate {
                    id: ruleTextDelegate

                    text: ruleLayout.text

                    activeFocusOnTab: true
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
