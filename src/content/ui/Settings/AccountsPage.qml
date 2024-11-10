// SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import org.kde.tokodon

FormCard.FormCardPage {
    id: root

    required property TokodonApplication application

    FormCard.FormCard {
        id: accountsCard

        Layout.topMargin: Kirigami.Units.largeSpacing

        Repeater {
            model: AccountManager
            delegate: FormCard.AbstractFormDelegate {
                id: delegate

                required property var account
                required property string description
                required property string displayName

                Layout.fillWidth: true
                onClicked: Window.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "AccountPage"), {
                    account: delegate.account
                })

                contentItem: RowLayout {
                    KirigamiComponents.Avatar {
                        source: delegate.account.identity.avatarUrl
                        name: delegate.displayName
                        Layout.rightMargin: Kirigami.Units.largeSpacing
                        implicitWidth: Kirigami.Units.iconSizes.medium
                        implicitHeight: Kirigami.Units.iconSizes.medium
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: delegate.displayName
                            textFormat: Text.RichText
                            elide: Text.ElideRight
                            wrapMode: Text.Wrap
                            maximumLineCount: 2
                            color: Kirigami.Theme.textColor
                        }

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: `${delegate.description} (${delegate.account.instanceName})`
                            color: Kirigami.Theme.disabledTextColor
                            font: Kirigami.Theme.smallFont
                            elide: Text.ElideRight
                        }
                    }

                    FormCard.FormArrow {
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        direction: Qt.RightArrow
                    }
                }
            }
        }

        FormCard.FormDelegateSeparator {
            below: addAccountDelegate
        }

        FormCard.FormButtonDelegate {
            id: addAccountDelegate
            action: Kirigami.Action {
                fromQAction: root.application.action('add_account')
            }
        }
    }
}
