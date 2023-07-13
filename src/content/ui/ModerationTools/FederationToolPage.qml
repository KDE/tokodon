// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

Kirigami.ScrollablePage {
    title: i18n("Federation Tool Page")

    Kirigami.PromptDialog {
        id: newDomainBlockDialog
        implicitWidth: Kirigami.Units.gridUnit * 20

        contentItem: Kirigami.ScrollablePage {
            padding: 0

            ColumnLayout {

                MobileForm.FormTextFieldDelegate {
                    id: domain
                    label: i18nc("@info:Enter the domain address of the domain block", "Domain*")
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormTextFieldDelegate {
                    id: publicComment
                    label: i18n("Public Comment")
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormTextFieldDelegate {
                    id: privateComment
                    label: i18n("Private Comment")
                }
                MobileForm.FormDelegateSeparator {}
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
                    Component.onCompleted: severityCombobox.currentIndex = severityCombobox.indexOfValue("silence");
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormCheckDelegate {
                    id: rejectMedia
                    text: i18n("Reject Media Files")
                    description: i18n("Removes locally stored media files and refuses to download any in the future. Irrelevant for suspensions")
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormCheckDelegate {
                    id: rejectReports
                    text: i18n("Reject Reports")
                    description: i18n("Ignore all reports coming from this domain. Irrelevant for suspensions")
                }
                MobileForm.FormDelegateSeparator {}
                MobileForm.FormCheckDelegate {
                    id: obfuscateReports
                    text: i18n("Obfuscate Domain Name")
                    description: i18n("Partially obfuscate the domain name in the list if advertising the list of domain limitations is enabled")
                }
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
                onTriggered: {
                    federationView.model.newDomainBlock(domain.text, severityCombobox.currentValue, publicComment.text, privateComment.text, rejectMedia.checked, rejectReports.checked, obfuscateReports.checked)
                    newDomainBlockDialog.close();
                    showPassiveNotification(i18n("New Domain Block Added"))
                }
            }
        ]
    }

    header: ColumnLayout {
        Layout.topMargin: Kirigami.Units.largeSpacing * 6
        Layout.bottomMargin: Kirigami.Units.largeSpacing * 6
        
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
            implicitWidth: parent.width/4
            model: [
                {
                    display: i18nc("@info:Filter out all the blocked domains", "Limited"),
                    value: "Limited"
                },
                {
                    display: i18nc("@info:Filter out all the allowed domains", "All"),
                    value: "All"
                },
            ]
            textRole: "display"
            valueRole: "value"
            Component.onCompleted: moderationCombobox.currentIndex = moderationCombobox.indexOfValue(federationView.model.moderation);
            onCurrentIndexChanged: federationView.model.moderation = model[currentIndex].value
        }
        Kirigami.Separator {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
        }
    }

    ListView {
        id: federationView
        model: FederationToolModel {}

        delegate: QQC2.ItemDelegate {
            id: delegate
            required property var index
            required property var id
            required property var domain
            required property var createdAt
            required property var severity
            required property var rejectMedia
            required property var rejectReports
            required property var publicComment
            required property var privateComment
            required property var obfuscate

            width: ListView.view.width

            onClicked: applicationWindow().pageStack.layers.push("./MainFederationToolPage.qml",
                            {
                                index: delegate.index,
                                model: federationView.model,
                                id: delegate.id,
                                domain: delegate.domain,
                                createdAt: delegate.createdAt,
                                severity: delegate.severity,
                                rejectMedia: delegate.rejectMedia,
                                rejectReports: delegate.rejectReports,
                                privateComment: delegate.privateComment,
                                publicComment: delegate.publicComment,
                                obfuscate: delegate.obfuscate,
                            },)

            contentItem: Kirigami.FlexColumn {
                spacing: 0
                maximumWidth: Kirigami.Units.gridLayout * 40

                RowLayout {
                    Layout.fillWidth: true
                    
                    ColumnLayout {
                        spacing: 0
                        Kirigami.Heading {
                            level: 2
                            text: delegate.domain
                            type: Kirigami.Heading.Type.Primary
                            Layout.alignment: Qt.AlignLeft
                        }
                        QQC2.Label {
                            font.pixelSize: Config.defaultFont.pixelSize + 1
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            color: Kirigami.Theme.disabledTextColor
                            text: delegate.severity
                            verticalAlignment: Text.AlignTop
                        }
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
                Kirigami.Separator {
                    Layout.fillWidth: true
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
            text: i18n("No Federations Found")
            visible: federationView.count === 0 && !federationView.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }

    footer: QQC2.ToolBar {
        
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }
            QQC2.Button {
                text: i18n("Add New Domain Block")
                icon.name: 'list-add'
                visible: true
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: {
                    newDomainBlockDialog.open()
                }
            }
        }
    }
}
