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

Kirigami.Page {
    id: root

    property string pageId

    title: AccountManager.selectedAccount.instanceName
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0
    topPadding: 0

    contentItem: ColumnLayout {
        spacing: 0

        QQC2.TabBar {
            id: tabBar

            Layout.fillWidth: true

            QQC2.TabButton {
                text: i18nc("@item:inmenu Profile Post Filter", "About")
            }
            QQC2.TabButton {
                text: i18nc("@item:inmenu Profile Post Filter", "Rules")
            }
            QQC2.TabButton {
                text: i18nc("@item:inmenu Profile Post Filter", "Announcements")
            }
            QQC2.TabButton {
                text: i18nc("@item:inmenu Profile Post Filter", "Privacy Policy")
            }
            QQC2.TabButton {
                text: i18nc("@item:inmenu Profile Post Filter", "Terms of Service")
            }
        }

        StackLayout {
            id: layout

            currentIndex: tabBar.currentIndex

            Layout.fillWidth: true
            Layout.fillHeight: true

            GenericServerInformationPage {
                kind: "extended_description"
            }
            FormCard.FormCardPage {
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
            AnnouncementsPage {}
            GenericServerInformationPage {
                kind: "privacy_policy"
            }
            GenericServerInformationPage {
                kind: "terms_of_service"
            }
        }
    }
}
