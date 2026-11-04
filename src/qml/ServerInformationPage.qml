// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.tokodon

FormCard.FormCardPage {
    id: root

    property string pageId

    title: AccountManager.selectedAccount.instanceName

    Component {
        id: rulesPage

        FormCard.FormCardPage {
            title: KI18n.i18nc("@title:window Server rules", "Rules")

            FormCard.FormCard {
                autoSeparators: true
                Layout.topMargin: Kirigami.Units.largeSpacing * 4

                Repeater {
                    model: RulesModel {
                        account: AccountManager.selectedAccount
                    }

                    delegate: FormCard.AbstractFormDelegate {
                        id: ruleDelegate

                        required text

                        background: null
                        contentItem: Kirigami.SelectableLabel {
                            text: ruleDelegate.text
                        }
                    }
                }
            }
        }
    }

    FormCard.FormCard {
        autoSeparators: true
        Layout.topMargin: Kirigami.Units.largeSpacing * 4

        FormCard.FormButtonDelegate {
            id: rulesButton

            text: KI18n.i18nc("@action:button", "Rules")
            onClicked: root.Kirigami.PageStack.push(rulesPage);
        }

        FormCard.FormButtonDelegate {
            id: announcementsButton

            text: KI18n.i18nc("@action:button", "Announcements")
            onClicked: root.Kirigami.PageStack.push(Qt.createComponent("org.kde.tokodon", "AnnouncementsPage"));
        }

        FormCard.FormButtonDelegate {
            id: privacyPolicyButton

            text: KI18n.i18nc("@action:button", "Privacy Policy")
            onClicked: root.Kirigami.PageStack.push(Qt.createComponent("org.kde.tokodon", "GenericServerInformationPage"), { title: KI18n.i18nc("@title:window", "Privacy Policy"), kind: "privacy_policy" });
        }

        FormCard.FormButtonDelegate {
            id: termsOfServiceButton

            text: KI18n.i18nc("@action:button", "Terms of Service")
            onClicked: root.Kirigami.PageStack.push(Qt.createComponent("org.kde.tokodon", "GenericServerInformationPage"), { title: KI18n.i18nc("@title:window", "Terms of Service"), kind: "terms_of_service" });
        }
    }

    ServerInformationCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 2

        kind: "extended_description"
    }
}
