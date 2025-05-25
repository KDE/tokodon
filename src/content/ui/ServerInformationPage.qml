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

    property string pageId

    title: AccountManager.selectedAccount.instanceName

    Component {
        id: rulesPage

        FormCard.FormCardPage {
            title: i18nc("@title:window Server rules", "Rules")

            FormCard.FormCard {
                Layout.topMargin: Kirigami.Units.largeSpacing * 4

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

                        FormCard.AbstractFormDelegate {
                            background: null
                            contentItem: Kirigami.SelectableLabel {
                                text: ruleLayout.text
                            }
                        }
                    }
                }
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 4

        FormCard.FormButtonDelegate {
            id: rulesButton

            text: i18nc("@action:button", "Rules")
            onClicked: pageStack.push(rulesPage);
        }

        FormCard.FormDelegateSeparator {
            above: rulesButton
            below: announcementsButton
        }

        FormCard.FormButtonDelegate {
            id: announcementsButton

            text: i18nc("@action:button", "Announcements")
            onClicked: pageStack.push(Qt.createComponent("org.kde.tokodon", "AnnouncementsPage"));
        }

        FormCard.FormDelegateSeparator {
            above: announcementsButton
            below: privacyPolicyButton
        }

        FormCard.FormButtonDelegate {
            id: privacyPolicyButton

            text: i18nc("@action:button", "Privacy Policy")
            onClicked: pageStack.push(Qt.createComponent("org.kde.tokodon", "GenericServerInformationPage"), { title: i18nc("@title:window", "Privacy Policy"), kind: "privacy_policy" });
        }

        FormCard.FormDelegateSeparator {
            above: privacyPolicyButton
            below: termsOfServiceButton
        }

        FormCard.FormButtonDelegate {
            id: termsOfServiceButton

            text: i18nc("@action:button", "Terms of Service")
            onClicked: pageStack.push(Qt.createComponent("org.kde.tokodon", "GenericServerInformationPage"), { title: i18nc("@title:window", "Terms of Service"), kind: "terms_of_service" });
        }
    }

    ServerInformationCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 2

        kind: "extended_description"
    }
}
