// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import org.kde.kmasto 1.0
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

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
        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Welcome to Tokodon")
                }

                MobileForm.FormTextFieldDelegate {
                    id: instanceUrl
                    label: i18n("Instance Url:")
                    onAccepted: continueButton.clicked()
                    inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                }

                MobileForm.FormDelegateSeparator { above: adminScopeDelegate }

                MobileForm.FormCheckDelegate {
                    id: adminScopeDelegate
                    text: i18n("Enable moderation tools")
                    description: i18n("Allow Tokodon to access moderation tools. Try disabling this if you have trouble logging into your server.")
                    checked: true
                }

                MobileForm.FormDelegateSeparator { above: continueButton }

                MobileForm.FormButtonDelegate {
                    id: continueButton
                    text: i18n("Continue")
                    onClicked: {
                        if (!instanceUrl.text) {
                            applicationWindow().showPassiveNotification(i18n("Instance URL must not be empty!"));
                            return;
                        }

                        const account = AccountManager.createNewAccount(instanceUrl.text, sslErrors.checked, adminScopeDelegate.checked);

                        account.registered.connect(() => {
                            const page = pageStack.layers.push('qrc:/content/ui/AuthorizationPage.qml', {
                                account: account,
                            });

                        });
                    }
                }
            }
        }

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18nc("@title:group Login page", "Network Settings")
                }

                MobileForm.FormSwitchDelegate {
                    id: sslErrors
                    text: i18nc("@option:check Login page", "Ignore SSL errors")
                }

                MobileForm.FormDelegateSeparator { above: proxySettingDelegate; below: sslErrors }

                MobileForm.FormButtonDelegate {
                    id: proxySettingDelegate
                    text: i18n("Proxy Settings")
                    onClicked: pageStack.layers.push('qrc:/content/ui/Settings/NetworkProxyPage.qml')
                }
            }
        }
    }
}
