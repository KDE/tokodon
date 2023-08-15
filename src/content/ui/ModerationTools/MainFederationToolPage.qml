// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

FormCard.FormCardPage {
    id: root

    property int index
    property var model
    property int id
    property string domain
    property date createdAt
    property string severity
    property bool rejectMedia
    property bool rejectReports
    property bool obfuscate
    property string privateComment
    property string publicComment

    title: root.domain

    actions.contextualActions: Kirigami.Action {
        icon.name: "edit-delete-remove"
        text: i18n("Remove Domain Block")
        onTriggered: {
            root.model.removeDomainBlock(root.index)
            showPassiveNotification(i18n("Domain Block Removed"))
            pageStack.layers.pop()
        }
    }

    data: Kirigami.PromptDialog {
        id: textPromptDialog

        title: i18n("Edit domain block")

        contentPadding: 0
        implicitWidth: Kirigami.Units.gridUnit * 20

        mainItem: ColumnLayout {
            spacing: 0

            FormCard.FormComboBoxDelegate {
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
            FormCard.FormDelegateSeparator {}
            FormCard.FormTextFieldDelegate {
                id: publicComment
                text: root.publicComment
                label: i18n("Public comment")
            }
            FormCard.FormDelegateSeparator {}
            FormCard.FormTextFieldDelegate {
                id: privateComment
                text: root.privateComment
                label: i18n("Private comment")
            }
            FormCard.FormDelegateSeparator { above: rejectMedia }
            FormCard.FormCheckDelegate {
                id: rejectMedia
                text: i18n("Reject media files")
                description: i18n("Removes locally stored media files and refuses to download any in the future. Irrelevant for suspensions")
                checked: root.rejectMedia
                onToggled: root.rejectMedia = rejectMedia.checked
            }
            FormCard.FormDelegateSeparator { below: rejectMedia; above: rejectReports }
            FormCard.FormCheckDelegate {
                id: rejectReports
                text: i18n("Reject reports")
                description: i18n("Ignore all reports coming from this domain. Irrelevant for suspensions")
                checked: root.rejectReports
                onToggled: root.rejectReports = rejectReports.checked
            }
            FormCard.FormDelegateSeparator { below: rejectReports; above: obfuscateReport }
            FormCard.FormCheckDelegate {
                id: obfuscateReports
                text: i18n("Obfuscate domain name")
                description: i18n("Partially obfuscate the domain name in the list if advertising the list of domain limitations is enabled")
                checked: root.obfuscate
                onToggled: root.obfuscate = obfuscateReports.checked
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

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormTextDelegate {
            text: i18n("Blocked at")
            description: root.createdAt.toLocaleDateString()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18n("Public comment")
            description: root.publicComment.length !== 0 ? root.publicComment : i18nc("@info: No public comment provided", "None")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18n("Private comment")
            description: root.privateComment.length !== 0 ? root.privateComment : i18nc("@info: No private comment provided", "None")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@info:The policy to be applied by this domain block", "Policy")
            description: root.severity
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@info:Whether to obfuscate public displays of this domain block", "Obfuscate")
            description: root.obfuscate
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@info:Whether to reject incoming media from this domain", "Reject media")
            description: root.rejectMedia
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@info:Whether to reject incoming reports from this domain", "Reject report")
            description: root.rejectReports
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
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    textPromptDialog.open()
                }
            }
        }
    }
}
