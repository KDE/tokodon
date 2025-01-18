// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
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

    title: AccountManager.selectedAccount.instanceName

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 4

        FormCard.FormButtonDelegate {
            text: i18nc("@action:button", "Announcements")
            onClicked: pageStack.push(Qt.createComponent("org.kde.tokodon", "AnnouncementsPage"));
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: i18nc("@action:button", "Privacy Policy")
            onClicked: pageStack.push(Qt.createComponent("org.kde.tokodon", "PrivacyPolicyPage"));
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Rules")
    }

    FormCard.FormCard {
        Repeater {
            model: RulesModel {
                account: AccountManager.selectedAccount
            }

            delegate: ColumnLayout {
                id: ruleLayout

                required property int index
                required property string text

                spacing: 0

                FormCard.FormDelegateSeparator {
                    visible: index !== 0
                    opacity: 0.5
                }

                FormCard.FormTextDelegate {
                    text: ruleLayout.text
                }
            }
        }
    }
}
