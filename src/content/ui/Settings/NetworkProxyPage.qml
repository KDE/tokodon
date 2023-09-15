// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard

import org.kde.tokodon
import org.kde.tokodon.private

FormCard.FormCardPage {
    title: i18nc("@title:window", "Network Proxy")
    property int currentType
    property bool proxyConfigChanged: false

    FormCard.FormHeader {
        title: i18n("Network Proxy")
    }

    FormCard.FormCard {
        FormCard.FormRadioDelegate {
            id: systemDefault
            text: i18n("System Default")
            checked: currentType === 0
            enabled: !Config.isProxyTypeImmutable
            onToggled: {
                currentType = 0
            }
        }

        FormCard.FormDelegateSeparator { below: systemDefault; above: http }

        FormCard.FormRadioDelegate {
            id: http
            text: i18n("HTTP")
            checked: currentType === 1
            enabled: !Config.isProxyTypeImmutable
            onToggled: {
                currentType = 1
            }
        }

        FormCard.FormDelegateSeparator { below: http; above: socks5 }

        FormCard.FormRadioDelegate {
            id: socks5
            text: i18n("Socks5")
            checked: currentType === 2
            enabled: !Config.isProxyTypeImmutable
            onToggled: {
                currentType = 2
            }
        }
    }

    FormCard.FormHeader {
        title: i18n("Proxy Settings")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: hostField
            label: i18n("Host")
            text: Config.proxyHost
            inputMethodHints: Qt.ImhUrlCharactersOnly
            onEditingFinished: {
                proxyConfigChanged = true
            }
        }
        FormCard.FormDelegateSeparator { below: hostField; above: portField }
        // we probably still need a FormSpinBoxDelegate
        FormCard.AbstractFormDelegate {
            Layout.fillWidth: true
            contentItem: RowLayout {
                QQC2.Label {
                    text: i18n("Port")
                    Layout.fillWidth: true
                }
                QQC2.SpinBox {
                    id: portField
                    value: Config.proxyPort
                    from: 0
                    to: 65536
                    validator: IntValidator {bottom: portField.from; top: portField.to}
                    textFromValue: function(value, locale) {
                        return value // it will add a thousands separator if we don't do this, not sure why
                    }
                    onValueChanged: {
                        proxyConfigChanged = true
                    }
                }
            }
        }
        FormCard.FormDelegateSeparator { below: portField; above: userField }
        FormCard.FormTextFieldDelegate {
            id: userField
            label: i18n("User")
            text: Config.proxyUser
            inputMethodHints: Qt.ImhUrlCharactersOnly
            onEditingFinished: {
                proxyConfigChanged = true
            }
        }
        FormCard.FormDelegateSeparator { below: userField; above: passwordField }
        FormCard.FormTextFieldDelegate {
            id: passwordField
            label: i18n("Password")
            text: Config.proxyPassword
            echoMode: TextInput.Password
            inputMethodHints: Qt.ImhUrlCharactersOnly
            onEditingFinished: {
                proxyConfigChanged = true
            }
        }
    }

    footer: QQC2.ToolBar {
        height: visible ? implicitHeight : 0
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }

            QQC2.Button  {
                text: i18n("Apply")
                enabled: currentType !== Config.proxyType || proxyConfigChanged
                onClicked: {
                    Config.proxyType = currentType
                    Config.proxyHost = hostField.text
                    Config.proxyPort = portField.value
                    Config.proxyUser = userField.text
                    Config.proxyPassword = passwordField.text
                    Config.save()
                    proxyConfigChanged = false
                    Controller.setApplicationProxy()
                }
            }
        }
    }

    Component.onCompleted: {
        currentType = Config.proxyType
    }
}
