// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import QtQuick.Layouts

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.tokodon

MastoPage {
    id: root

    title: KI18n.i18nc("@title:window", "Rules")

    required property AbstractAccount account

    data: [
        Connections {
            target: Controller
            function onNetworkErrorOccurred(error) {
                message.text = KI18n.i18nc("@info:status Network status", "Failed to contact server: %1. Please check your proxy settings.", error)
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
            text: KI18n.i18n("Proxy Settings")
            icon.name: "settings-configure"
            onTriggered: (root.QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow).pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "NetworkProxyPage"))
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Rules")
    }

    FormCard.FormCard {
        autoSeparators: true

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

            delegate: FormCard.FormTextDelegate {
                id: ruleTextDelegate

                required text

                function forceFocus(): void {
                    ruleTextDelegate.forceActiveFocus();
                }

                activeFocusOnTab: true
            }
        }
    }

    FormCard.FormCard {
        autoSeparators: true
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormButtonDelegate {
            id: agreeButton

            text: KI18n.i18nc("@action:button Agree to server rules", "Agree")
            icon.name: "dialog-ok"

            onClicked: {
                (root.QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow).pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "RegistrationPage"), {
                    account: account,
                });
            }
        }

        FormCard.FormButtonDelegate {
            id: disagreeButton

            text: KI18n.i18nc("@action:button Disagree to server rules", "Disagree")
            icon.name: "dialog-cancel"

            onClicked: (root.QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow).pageStack.layers.pop()
        }
    }
}
