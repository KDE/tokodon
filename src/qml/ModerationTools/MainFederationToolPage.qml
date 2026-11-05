// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

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
        text: KI18n.i18n("Remove Domain Block")
        onTriggered: {
            root.model.removeDomainBlock(root.index)
            root.QQC2.ApplicationWindow.window.showPassiveNotification(KI18n.i18n("Domain Block Removed"))
            root.Kirigami.PageStack.pop()
        }
    }

    data: FormCard.FormCardDialog {
        id: textPromptDialog
        parent: root.QQC2.Overlay.overlay

        title: KI18n.i18n("Edit domain block")
        autoSeparators: true

        standardButtons: FormCard.FormCardDialog.Ok | FormCard.FormCardDialog.Cancel

        Component.onCompleted: {
            const updateButton = standardButton(FormCard.FormCardDialog.Ok);
            updateButton.text = KI18n.i18nc("@info:Button to update the domain block with new values", "Update Block");
            updateButton.icon.name = "view-refresh";
        }

        onAccepted: {
            root.privateComment = privateComment.text
            root.publicComment = publicComment.text
            root.model.updateDomainBlock(root.index, severityCombobox.currentValue, publicComment.text, privateComment.text, rejectMedia.checked, rejectReports.checked, obfuscateReports.checked)
            root.QQC2.ApplicationWindow.window.showPassiveNotification(KI18n.i18n("Domain block updated"))
            textPromptDialog.close();
        }

        onRejected: textPromptDialog.close();

        FormCard.FormComboBoxDelegate {
            id: severityCombobox
            text: KI18n.i18n("Moderation")
            textRole: "display"
            valueRole: "value"
            model: [
                {
                    display: KI18n.i18nc("@info:Filter out all the allowed domains", "Silence"),
                    value: "silence"
                },
                {
                    display: KI18n.i18nc("@info:Filter out all the blocked domains", "Suspend"),
                    value: "suspend"
                },
                {
                    display: KI18n.i18nc("@info:Filter out all the blocked domains", "None"),
                    value: "noop"
                },
            ]
            onCurrentIndexChanged: root.severity = model[currentIndex].value
            Component.onCompleted: severityCombobox.currentIndex = severityCombobox.indexOfValue(root.severity);
        }

        FormCard.FormTextFieldDelegate {
            id: publicComment
            text: root.publicComment
            label: KI18n.i18n("Public comment")
        }

        FormCard.FormTextFieldDelegate {
            id: privateComment
            text: root.privateComment
            label: KI18n.i18n("Private comment")
        }

        FormCard.FormCheckDelegate {
            id: rejectMedia
            text: KI18n.i18n("Reject media files")
            description: KI18n.i18n("Removes locally stored media files and refuses to download any in the future. Irrelevant for suspensions")
            checked: root.rejectMedia
            onToggled: root.rejectMedia = rejectMedia.checked
        }

        FormCard.FormCheckDelegate {
            id: rejectReports
            text: KI18n.i18n("Reject reports")
            description: KI18n.i18n("Ignore all reports coming from this domain. Irrelevant for suspensions")
            checked: root.rejectReports
            onToggled: root.rejectReports = rejectReports.checked
        }

        FormCard.FormCheckDelegate {
            id: obfuscateReports
            text: KI18n.i18n("Obfuscate domain name")
            description: KI18n.i18n("Partially obfuscate the domain name in the list if advertising the list of domain limitations is enabled")
            checked: root.obfuscate
            onToggled: root.obfuscate = obfuscateReports.checked
        }
    }

    FormCard.FormCard {
        autoSeparators: true
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormTextDelegate {
            text: KI18n.i18n("Blocked at")
            description: root.createdAt.toLocaleDateString()
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18n("Public comment")
            description: root.publicComment.length !== 0 ? root.publicComment : KI18n.i18nc("@info: No public comment provided", "None")
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18n("Private comment")
            description: root.privateComment.length !== 0 ? root.privateComment : KI18n.i18nc("@info: No private comment provided", "None")
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@info:The policy to be applied by this domain block", "Policy")
            description: root.severity
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@info:Whether to obfuscate public displays of this domain block", "Obfuscate")
            description: root.obfuscate
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@info:Whether to reject incoming media from this domain", "Reject media")
            description: root.rejectMedia
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@info:Whether to reject incoming reports from this domain", "Reject reports")
            description: root.rejectReports
        }
    }

    footer: QQC2.ToolBar {
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }
            QQC2.Button {
                text: KI18n.i18n("Edit Domain Block")
                icon.name: 'cell_edit'
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    textPromptDialog.open()
                }
            }
        }
    }
}
