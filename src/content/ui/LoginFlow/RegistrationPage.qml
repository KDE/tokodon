// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigamiaddons.formcard 1 as FormCard

import org.kde.tokodon

MastoPage {
    id: root

    title: i18nc("@title:window", "Registration")

    required property var account

    data: [
        Connections {
            target: Controller
            function onNetworkErrorOccurred(error) {
                message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your proxy settings.", error)
                message.visible = true;
            }
        },
        Connections {
            target: root.account

            function onRegistrationError(json) {
                const obj = JSON.parse(json);

                if (obj.error) {
                    message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your proxy settings.", obj.error)
                    message.visible = true;
                    return;
                }

                const details = obj.details;

                for (const key in details) {
                    let targetItem;
                    let targetName;
                    if (key === "email") {
                        targetItem = usernameField;
                        targetName = usernameField.label;
                    } else if (key === "email") {
                        targetItem = emailField;
                        targetName = emailField.label;
                    } else if (key === "password") {
                        targetItem = passwordField;
                        targetName = passwordField.label;
                    } else if (key === "reason") {
                        targetItem = reasonField;
                        targetName = reasonField.label;
                    }

                    let errorMessage = "";

                    for (const error in details[key]) {
                        if (error > 0) {
                            errorMessage += "\n";
                        }
                        errorMessage += targetName + " " + details[key][error].description;
                    }

                    targetItem.status = Kirigami.MessageType.Error;
                    targetItem.statusMessage = errorMessage;
                }
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

    Component.onCompleted: usernameField.forceActiveFocus()

    FormCard.FormHeader {
        title: i18nc("@title:group", "Register")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: usernameField
            label: i18n("Username")
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
            onAccepted: emailField.forceActiveFocus()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: emailField
            label: i18n("Email Address")
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
            onAccepted: passwordField.forceActiveFocus()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: passwordField
            label: i18n("Password")
            echoMode: TextInput.Password
            inputMethodHints: Qt.ImhHiddenText | Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
            onAccepted: reasonField.forceActiveFocus()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: reasonField
            label: i18n("Reason")
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormButtonDelegate {
            id: continueButton
            text: i18n("Register")
            onClicked: {
                root.account.registerAccount(usernameField.text, emailField.text, passwordField.text, true, "en", reasonField.text);

                account.authenticated.connect(() => {
                    pageStack.layers.clear();
                    pageStack.replace(mainTimeline, {
                        name: "home"
                    });
                    Window.window.close();
                });
            }
        }
    }
}
