// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as Components

import org.kde.tokodon

MastoPage {
    title: i18nc("@title:window", "Login")

    data: Connections {
        target: Controller
        function onNetworkErrorOccurred(error) {
            message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your proxy settings.", error)
            message.visible = true;
        }
    }

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

    Component.onCompleted: instanceUrl.forceActiveFocus()

    FormCard.FormHeader {
        title: i18nc("@title:group", "Login")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: instanceUrl
            label: i18n("Server URL:")
            placeholderText: i18n("mastodon.social")
            onAccepted: continueButton.clicked()
            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        }

        FormCard.FormDelegateSeparator { above: adminScopeDelegate }

        FormCard.FormCheckDelegate {
            id: adminScopeDelegate
            text: i18n("Enable moderation tools")
            description: i18n("Allow Tokodon to access moderation tools. Try disabling this if you have trouble logging in.")
            checked: true
        }

        FormCard.FormDelegateSeparator { above: continueButton }

        FormCard.FormButtonDelegate {
            id: continueButton
            text: i18n("Continue")
            onClicked: {
                instanceUrl.statusMessage = "";

                if (!instanceUrl.text) {
                    instanceUrl.status = Kirigami.MessageType.Error;
                    instanceUrl.statusMessage = i18n("Server URL must not be empty!");
                    return;
                }

                const account = AccountManager.createNewAccount(instanceUrl.text, sslErrors.checked, adminScopeDelegate.checked);

                account.registered.connect(() => {
                    Window.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "AuthorizationPage"), {
                        account: account,
                    });
                });
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Network Settings")
    }

    FormCard.FormCard {
        FormCard.FormSwitchDelegate {
            id: sslErrors
            text: i18nc("@option:check Login page", "Ignore SSL errors")
        }

        FormCard.FormDelegateSeparator { above: proxySettingDelegate; below: sslErrors }

        FormCard.FormButtonDelegate {
            id: proxySettingDelegate
            text: i18n("Proxy Settings")
            onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "NetworkProxyPage"))
        }
    }
}
