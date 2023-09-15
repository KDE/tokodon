// SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import Qt.labs.platform 1.1

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import org.kde.tokodon

FormCard.FormCard {
    id: accountsCard

    Repeater {
        id: accountRepeater

        model: AccountManager
        delegate: FormCard.AbstractFormDelegate {
            Layout.fillWidth: true
            onClicked: applicationWindow().pageStack.layers.push("./ProfileEditor.qml", {
                account: model.account,
            }, {
                title: i18n("Account editor")
            })

            contentItem: RowLayout {
                KirigamiComponents.Avatar {
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
                        textFormat: Text.RichText
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
                    }
                }

                FormCard.FormArrow {
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    direction: FormCard.FormArrow.Right
                }
            }
        }
    }

    FormCard.FormDelegateSeparator { above: addAccountDelegate; below: accountRepeater.itemAt(accountRepeater.count - 1) }

    FormCard.FormButtonDelegate {
        id: addAccountDelegate
        text: i18n("Add Account")
        icon.name: "list-add"
        onClicked: pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "LoginPage"), {}, {
            width: pageStack.width - Kirigami.Units.gridUnit * 5,
            height: pageStack.height - Kirigami.Units.gridUnit * 5,
        })
    }
}
