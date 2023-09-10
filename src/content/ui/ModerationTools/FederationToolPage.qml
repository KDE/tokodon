// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.kirigamiaddons.labs.components 1.0 as Components
import org.kde.kirigamiaddons.delegates 1.0 as Delegates

Kirigami.ScrollablePage {
    id: root

    property bool isDomainBlock

    actions: [
        Kirigami.Action {
            icon.name: 'list-add'
            text: i18n("Add New Domain Block")
            visible: isDomainBlock
            onTriggered: newDomainBlockDialog.open()
        },
        Kirigami.Action {
            icon.name: 'list-add'
            text: i18n("Allow Federation with Domain")
            visible: !isDomainBlock
            onTriggered: newDomainAllowDialog.open()
        }
    ]

    Kirigami.PromptDialog {
        id: allowedDomainInfo

        property int index
        property string domainName
        property date createdAt
        title: i18n("Allowed Domain Info")

        implicitWidth: Kirigami.Units.gridUnit * 20

        contentItem: ColumnLayout {
            FormCard.FormTextDelegate {
                text: i18nc("@info:The domain that is allowed to federate", "Domain")
                description: allowedDomainInfo.domainName
                Layout.fillWidth: true
            }
            FormCard.FormDelegateSeparator {}
            FormCard.FormTextDelegate {
                text: i18nc("@info:Time when the domain was allowed to federate.", "Created at")
                description: allowedDomainInfo.createdAt.toDateString()
                Layout.fillWidth: true
            }
        }

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: i18n("Disallow Federation with Domain")
                icon.name: "edit-delete-remove"
                onTriggered: {
                    federationView.model.removeAllowedDomain(root.index)
                    showPassiveNotification(i18n("Disallowed federation with the domain"))
                    allowedDomainInfo.close()
                }
            }
        ]
    }

    Kirigami.PromptDialog {
        id: newDomainBlockDialog

        title: i18n("Add Domain Block")

        contentPadding: 0
        implicitWidth: Kirigami.Units.gridUnit * 20

        mainItem: ColumnLayout {
            spacing: 0

            FormCard.FormTextFieldDelegate {
                id: domain
                label: i18nc("@info:Enter the domain address of the domain block", "Domain*")
            }
            FormCard.FormDelegateSeparator {}
            FormCard.FormTextFieldDelegate {
                id: publicComment
                label: i18n("Public comment")
            }
            FormCard.FormDelegateSeparator {}
            FormCard.FormTextFieldDelegate {
                id: privateComment
                label: i18n("Private Comment")
            }
            FormCard.FormDelegateSeparator {}
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
                Component.onCompleted: severityCombobox.currentIndex = severityCombobox.indexOfValue("silence");
            }
            FormCard.FormDelegateSeparator { above: rejectMedia }
            FormCard.FormCheckDelegate {
                id: rejectMedia
                text: i18n("Reject media files")
                description: i18n("Removes locally stored media files and refuses to download any in the future. Irrelevant for suspensions")
            }
            FormCard.FormDelegateSeparator { below: rejectMedia; above: rejectReports }
            FormCard.FormCheckDelegate {
                id: rejectReports
                text: i18n("Reject reports")
                description: i18n("Ignore all reports coming from this domain. Irrelevant for suspensions")
            }
            FormCard.FormDelegateSeparator { below: rejectReports; above: obfuscateReport }
            FormCard.FormCheckDelegate {
                id: obfuscateReport
                text: i18n("Obfuscate domain name")
                description: i18n("Partially obfuscate the domain name in the list if advertising the list of domain limitations is enabled")
            }
        }

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@info:Cancel button to close the dailog", "Cancel")
                icon.name: "dialog-cancel"
                onTriggered: newDomainBlockDialog.close();
            },
            Kirigami.Action {
                text: i18nc("@info:Button to create a domain block", "Create Block")
                icon.name: "checkbox"
                enabled: domain.text.length > 0
                onTriggered: {
                    federationView.model.newDomainBlock(domain.text, severityCombobox.currentValue, publicComment.text, privateComment.text, rejectMedia.checked, rejectReports.checked, obfuscateReports.checked)
                    showPassiveNotification(i18n("New domain block added"))
                    newDomainBlockDialog.close();
                }
            }
        ]
    }

    Kirigami.PromptDialog {
        id: newDomainAllowDialog
        title: i18n("Allow Federation with Domain")

        contentPadding: 0

        mainItem: FormCard.FormTextFieldDelegate {
            id: newAllowedDomain
            label: i18nc("@info:This domain will be able to fetch data from this server and incoming data from it will be processed and stored", "Domain*")
        }

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@info:Cancel button to close the dailog", "Cancel")
                icon.name: "dialog-cancel"
                onTriggered: newDomainAllowDialog.close();
            },
            Kirigami.Action {
                text: i18nc("@info:Button to create a domain block", "Create Block")
                icon.name: "checkbox"
                onTriggered: {
                    federationView.model.newDomainAllow(newAllowedDomain.text)
                    showPassiveNotification(i18n("New Allowed Domain Added"))
                    newDomainAllowDialog.close();
                }
            }
        ]
    }

    header: ColumnLayout {
        Layout.topMargin: Kirigami.Units.largeSpacing * 6
        Layout.bottomMargin: Kirigami.Units.largeSpacing * 6

        Components.Banner {
            topPadding: Kirigami.Units.largeSpacing
            text: i18n("Allowed domain option is available for instances with limited federation mode enabled")
            Layout.fillWidth: true
            visible: true
        }

        Kirigami.Heading {
            level: 4
            text: i18nc("@info:Choose between allowed and limited domains", "Moderation")
            type: Kirigami.Heading.Type.Primary
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
        }
        QQC2.ComboBox {
            id: moderationCombobox
            Layout.alignment: Qt.AlignCenter
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
            implicitWidth: Kirigami.Units.gridUnit * 8
            model: [
                {
                    display: i18nc("@info:Filter out all the blocked domains", "Blocked domains"),
                    value: FederationToolModel.BlockedDomains
                },
                {
                    display: i18nc("@info:Filter out all the allowed domains in limited federation mode", "Allowed domains"),
                    value: FederationToolModel.AllowedDomains
                },
            ]
            textRole: "display"
            valueRole: "value"
            Component.onCompleted: moderationCombobox.currentIndex = moderationCombobox.indexOfValue(federationView.model.federationAction);
            onCurrentIndexChanged: {
                federationView.model.federationAction = model[currentIndex].value;
                isDomainBlock = (model[currentIndex].value === FederationToolModel.BlockedDomains);
            }
        }
        Kirigami.Separator {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
        }
    }

    ListView {
        id: federationView

        model: FederationToolModel {}
        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property int index
            required property int id
            required property string domain
            required property date createdAt
            required property var severity
            required property bool rejectMedia
            required property bool rejectReports
            required property string publicComment
            required property string privateComment
            required property bool obfuscate

            width: ListView.view.width

            onClicked: if (root.isDomainBlock) {
               applicationWindow().pageStack.layers.push("./MainFederationToolPage.qml", {
                    index: delegate.index,
                    model: federationView.model,
                    id: delegate.id,
                    domain: delegate.domain,
                    createdAt: delegate.createdAt,
                    severity: delegate.severity,
                    rejectMedia: delegate.rejectMedia,
                    rejectReports: delegate.rejectReports,
                    obfuscate: delegate.obfuscate,
                    privateComment: delegate.privateComment,
                    publicComment: delegate.publicComment,
                });
            } else {
                allowedDomainInfo.index = delegate.index;
                allowedDomainInfo.domainName = delegate.domain;
                allowedDomainInfo.createdAt = delegate.createdAt;
                allowedDomainInfo.open();
            }

            text: delegate.domain

            contentItem: RowLayout {
                Delegates.SubtitleContentItem {
                    itemDelegate: delegate
                    subtitle: root.isDomainBlock ? delegate.severity : i18n("Allowed for federation")
                }

                Item {
                    Layout.fillWidth: true
                }

                Kirigami.Heading {
                    level: 4
                    text: delegate.createdAt.toLocaleDateString()
                    type: Kirigami.Heading.Type.Secondary
                    Layout.alignment: Qt.AlignRight
                }
            }
        }

        QQC2.ProgressBar {
            visible: federationView.model.loading && federationView.count === 0
            anchors.centerIn: parent
            indeterminate: true
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No federations found")
            visible: federationView.count === 0 && !federationView.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
