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
                    onAccepted: username.forceActiveFocus()
                    inputMethodHints: Qt.ImhUrlCharactersOnly
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormTextFieldDelegate {
                    id: username
                    label: i18n("Username:")
                    onAccepted: continueButton.clicked()
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormSwitchDelegate {
                    id: sslErrors
                    text: i18n("Ignore ssl errors")
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormButtonDelegate {
                    id: continueButton
                    text: i18n("Continue")
                    onClicked: {
                        if (!instanceUrl.text || !username.text) {
                            applicationWindow().showPassiveNotification(i18n("Instance URL and username must not be empty!"));
                            return;
                        }

                        pageStack.layers.push('qrc:/content/ui/AuthorizationPage.qml', {
                            account: AccountManager.createNewAccount(username.text, instanceUrl.text, sslErrors.checked)
                        });
                    }
                }
            }
        }
    }
}
