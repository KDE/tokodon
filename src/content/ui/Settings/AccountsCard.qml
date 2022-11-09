// SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

import org.kde.sonnet 1.0 as Sonnet
import org.kde.kirigami 2.15 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

import org.kde.kmasto 1.0

MobileForm.FormCard {
    Layout.topMargin: Kirigami.Units.largeSpacing
    Layout.fillWidth: true
    contentItem: ColumnLayout {
        spacing: 0
        MobileForm.FormCardHeader {
            title: i18n("Accounts")
        }

        Repeater {
            model: AccountManager
            delegate: MobileForm.FormButtonDelegate {
                //onClicked: pageSettingStack.pushDialogLayer("qrc:/AccountEditorPage.qml", {
                //    connection: model.connection
                //}, {
                //    title: i18n("Account editor")
                //})

                contentItem: RowLayout {
                    Kirigami.Avatar {
                        source: model.account.identity.avatarUrl
                        name: model.display
                        Layout.rightMargin: Kirigami.Units.largeSpacing
                        implicitWidth: Kirigami.Units.iconSizes.medium
                        implicitHeight: Kirigami.Units.iconSizes.medium
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: model.display
                            textFormat: Text.PlainText
                            elide: Text.ElideRight
                            wrapMode: Text.Wrap
                            maximumLineCount: 2
                            color: Kirigami.Theme.textColor
                        }

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: `${model.description} (${model.account.instanceName})`
                            color: Kirigami.Theme.disabledTextColor
                            font: Kirigami.Theme.smallFont
                            elide: Text.ElideRight
                        }
                    }

                    QQC2.ToolButton {
                        text: i18n("Logout")
                        icon.name: "im-kick-user"
                        onClicked: {
                            AccountManager.removeAccount(model.account)
                            if (!AccountManager.hasAccount) {
                                settingsPage.closeDialog()
                            }
                        }
                    }

                    // TODO implement account editing
                    //MobileForm.FormArrow {
                    //    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    //    direction: MobileForm.FormArrow.Right
                    //}
                }
            }
        }

        MobileForm.FormDelegateSeparator { below: addAccountDelegate }

        MobileForm.FormButtonDelegate {
            id: addAccountDelegate
            text: i18n("Add Account")
            icon.name: "list-add"
            onClicked: pageStack.pushDialogLayer("qrc:/content/ui/LoginPage.qml", {}, {
                width: pageStack.width - Kirigami.Units.gridUnit * 5,
                height: pageStack.height - Kirigami.Units.gridUnit * 5,
            })
        }
    }
}
