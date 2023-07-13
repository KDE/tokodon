// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

Kirigami.ScrollablePage {
    id: root

    property int index
    property var model
    property var id
    property var domain
    property var createdAt
    property var severity
    property var rejectMedia
    property var rejectReports
    property var privateComment
    property var publicComment
    property var obfuscate

    title: root.domain

    actions {
        main: Kirigami.Action {
            icon.name: "edit-delete-remove"
            text: i18n("Remove Domain Block")
            onTriggered: {
                root.model.removeDomainBlock(root.index)
                showPassiveNotification(i18n("Removed Domain Block"))
                pageStack.layers.pop()
            }
        }
    }

    Kirigami.PromptDialog {
        id: textPromptDialog
        implicitWidth: Kirigami.Units.gridUnit * 20

        contentItem: Kirigami.ScrollablePage {
            padding: 0

            ColumnLayout {
                MobileForm.FormComboBoxDelegate {
                    id: severityCombobox
                    text: i18n("Moderation")
                    textRole: "display"
                    valueRole: "value"
                    model: [
                        {
                            display: i18nc("@info:Filter out all the allowed domains", "Silence"),
                            value: "silence"
                        },
                        {
                            display: i18nc("@info:Filter out all the blocked domains", "Suspend"),
                            value: "suspend"
                        },
                        {
                            display: i18nc("@info:Filter out all the blocked domains", "None"),
                            value: "noop"
                        },
                    ]
                    onCurrentIndexChanged: root.severity = model[currentIndex].value
                    Component.onCompleted: severityCombobox.currentIndex = severityCombobox.indexOfValue(root.severity);
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormTextFieldDelegate {
                    id: publicComment
                    text: root.publicComment
                    label: i18n("Public Comment")
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormTextFieldDelegate {
                    id: privateComment
                    text: root.privateComment
                    label: i18n("Private Comment")
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormCheckDelegate {
                    id: rejectMedia
                    text: i18n("Reject Media Files")
                    description: i18n("Removes locally stored media files and refuses to download any in the future. Irrelevant for suspensions")
                    checked: root.rejectMedia
                    onToggled: root.rejectMedia = rejectMedia.checked
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormCheckDelegate {
                    id: rejectReports
                    text: i18n("Reject Reports")
                    description: i18n("Ignore all reports coming from this domain. Irrelevant for suspensions")
                    checked: root.rejectReports
                    onToggled: root.rejectReports = rejectReports.checked
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormCheckDelegate {
                    id: obfuscateReports
                    text: i18n("Obfuscate Domain Name")
                    description: i18n("Partially obfuscate the domain name in the list if advertising the list of domain limitations is enabled")
                    checked: root.obfuscate
                    onToggled: root.obfuscate = obfuscateReports.checked
                }
            }
        }
        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@info:Cancel button to close the dailog", "Cancel")
                icon.name: "dialog-cancel"
                onTriggered: textPromptDialog.close();
            },
            Kirigami.Action {
                text: i18nc("@info:Button to update the domain block with new values", "Update Block")
                icon.name: "view-refresh"
                onTriggered: {
                    root.privateComment = privateComment.text
                    root.publicComment = publicComment.text
                    root.model.updateDomainBlock(root.index, severityCombobox.currentValue, publicComment.text, privateComment.text, rejectMedia.checked, rejectReports.checked, obfuscateReports.checked)
                    textPromptDialog.close();
                    showPassiveNotification(i18n("Domain block updated"))
                }
            }
        ]
    }

    ColumnLayout {
        id: layout
        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            contentItem: ColumnLayout {
                spacing: 0
                MobileForm.FormTextDelegate {
                    visible: true
                    text: i18n("Blocked At")
                    description: root.createdAt.toLocaleDateString()
                }
                MobileForm.FormTextDelegate {
                    visible: true
                    text: i18n("Public Comment")
                    description: root.publicComment !== "" ? root.publicComment : i18nc("@info: No public comment provided","None")
                }
                MobileForm.FormTextDelegate {
                    visible: true
                    text: i18n("Private Comment")
                    description: root.privateComment !== "" ? root.privateComment : i18nc("@info: No private comment provided","None")
                }
                MobileForm.FormTextDelegate {
                    visible: true
                    text: i18nc("@info:The policy to be applied by this domain block","Policy")
                    description: root.severity
                }
                MobileForm.FormTextDelegate {
                    visible: true
                    text: i18nc("@info:Whether to obfuscate public displays of this domain block","Obfuscate")
                    description: root.obfuscate
                }
                MobileForm.FormTextDelegate {
                    visible: true
                    text: i18nc("@info:Whether to obfuscate public displays of this domain block","Reject Media")
                    description: root.rejectMedia
                }
                MobileForm.FormTextDelegate {
                    visible: true
                    text: i18nc("@info:Whether to obfuscate public displays of this domain block","Reject Report")
                    description: root.rejectReports
                }
            }
        }
    }

    footer: QQC2.ToolBar {

        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }
            QQC2.Button {
                text: i18n("Edit Domain Block")
                icon.name: 'cell_edit'
                visible: true
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    textPromptDialog.open()
                }
            }
        }
    }
}
