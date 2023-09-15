// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models
import org.kde.tokodon
import org.kde.kirigamiaddons.formcard 1 as FormCard

MastoPage {
    objectName: 'loginPage'
    title: i18n("Login")

    leftPadding: 0
    rightPadding: 0

    Connections {
        target: Controller
        function onNetworkErrorOccurred(error) {
            applicationWindow().showPassiveNotification(i18nc("@info:status Network status", "Failed to contact server: %1. Please check your settings.", error));
        }
    }

    ColumnLayout {
        width: parent.width

        FormCard.FormHeader {
            title: i18n("Welcome to Tokodon")
        }

        FormCard.FormCard {
            FormCard.FormTextFieldDelegate {
                id: instanceUrl
                label: i18n("Instance Url:")
                onAccepted: continueButton.clicked()
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
            }

            FormCard.FormDelegateSeparator { above: adminScopeDelegate }

            FormCard.FormCheckDelegate {
                id: adminScopeDelegate
                text: i18n("Enable moderation tools")
                description: i18n("Allow Tokodon to access moderation tools. Try disabling this if you have trouble logging into your server.")
                checked: true
            }

            FormCard.FormDelegateSeparator { above: continueButton }

            FormCard.FormButtonDelegate {
                id: continueButton
                text: i18n("Continue")
                onClicked: {
                    if (!instanceUrl.text) {
                        applicationWindow().showPassiveNotification(i18n("Instance URL must not be empty!"));
                        return;
                    }

                    const account = AccountManager.createNewAccount(instanceUrl.text, sslErrors.checked, adminScopeDelegate.checked);

                    account.registered.connect(() => {
                        const page = pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "AuthorizationPage"), {
                            account: account,
                        });

                    });
                }
            }
        }

        FormCard.FormHeader {
            title: i18nc("@title:group Login page", "Network Settings")
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
                onClicked: pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "NetworkProxyPage"))
            }
        }
    }
}
