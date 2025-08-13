// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.delegates 1 as Delegates

import "../PostDelegate"

Kirigami.ScrollablePage {
    id: root

    function reset() {
        username.text = ""
        displayName.text = ""
        email.text = ""
        ip.text = ""
        accountView.model.username = ""
        accountView.model.displayName = ""
        accountView.model.email = ""
        accountView.model.ip = ""
    }

    data: Kirigami.PromptDialog {
        id: textPromptDialog

        title: i18n("Advanced Search")

        leftPadding: 0
        rightPadding: 0

        implicitWidth: Kirigami.Units.gridUnit * 20

        contentItem: ColumnLayout {
            FormCard.FormTextFieldDelegate {
                id: username
                label: i18nc("@info:placeholder Username for searching accounts", "Username:")
            }
            FormCard.FormDelegateSeparator {}
            FormCard.FormTextFieldDelegate {
                id: displayName
                label: i18nc("@info:placeholder display name for searching accounts", "Display Name:")
            }
            FormCard.FormDelegateSeparator {}
            FormCard.FormTextFieldDelegate {
                id: email
                label: i18nc("@info:placeholder email for searching accounts", "Email:")
            }
            FormCard.FormDelegateSeparator {}
            FormCard.FormTextFieldDelegate {
                id: ip
                label: i18nc("@info:placeholder ip for searching accounts", "IP:")
            }
        }

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@info:Search button to initiate searching accounts", "Search")
                icon.name: "search"
                onTriggered: {
                    accountView.model.username = username.text
                    accountView.model.displayName = displayName.text
                    accountView.model.email = email.text
                    accountView.model.ip = ip.text
                    textPromptDialog.close();
                }
            },
            Kirigami.Action {
                text: i18nc("@info:Reset button to reset all the text fields", "Reset")
                icon.name: "edit-clear"
                onTriggered: reset()
            },
            Kirigami.Action {
                text: i18nc("@info:Cancel button to close the dailog", "Cancel")
                icon.name: "dialog-cancel"
                onTriggered: textPromptDialog.close();

            }
        ]
    }

    header: ColumnLayout {
        RowLayout {
            id: filterOptions
            spacing: Kirigami.Units.largeSpacing
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            ColumnLayout {
                Kirigami.Heading {
                    level: 4
                    text: i18nc("@info:Location combobox to choose location filters", "Location")
                    type: Kirigami.Heading.Type.Primary
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }
                QQC2.ComboBox {
                    Layout.fillWidth: true
                    id: locationCombobox
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    model: [
                        {
                            display: i18nc("@info:Filter out accounts from any location", "All"),
                            value: ""
                        },
                        {
                            display: i18nc("@info:Filter out local accounts", "Local"),
                            value: "local"
                        },
                        {
                            display: i18nc("@info:Filter out remote accounts", "Remote"),
                            value: "remote"
                        },
                    ]
                    textRole: "display"
                    valueRole: "value"
                    Component.onCompleted: locationCombobox.currentIndex = locationCombobox.indexOfValue(accountView.model.location);
                    onCurrentIndexChanged: accountView.model.location = model[currentIndex].value
                }
            }

            ColumnLayout {
                Kirigami.Heading {
                    level: 4
                    text: i18nc("@info:Moderation Status combobox to choose between different moderation status filters", "Moderation Status")
                    type: Kirigami.Heading.Type.Primary
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                QQC2.ComboBox {
                    Layout.fillWidth: true
                    id: moderationStatusCombobox
                    model: [
                        {
                            display: i18nc("@info:Filter out accounts with any moderation status", "All"),
                            value: ""
                        },
                        {
                            display: i18nc("@info:Filter out accounts with active moderation status", "Active"),
                            value: "active"
                        },
                        {
                            display: i18nc("@info:Filter out accounts with pending moderation status", "Pending"),
                            value: "pending"
                        },
                        {
                            display: i18nc("@info:Filter out accounts with disabled moderation status", "Disabled"),
                            value: "disabled"
                        },
                        {
                            display: i18nc("@info:Filter out accounts with silenced moderation status", "Silenced"),
                            value: "silenced"
                        },
                        {
                            display: i18nc("@info:Filter out accounts with suspended moderation status", "Suspended"),
                            value: "suspended"
                        }
                    ]
                    textRole: "display"
                    valueRole: "value"
                    Component.onCompleted: moderationStatusCombobox.currentIndex = moderationStatusCombobox.indexOfValue(accountView.model.moderationStatus);
                    onCurrentIndexChanged: accountView.model.moderationStatus = model[currentIndex].value
                }
            }
            ColumnLayout {
                Kirigami.Heading {
                    level: 4
                    text: i18nc("@info:Role combobox to choose between different role filters", "Role")
                    type: Kirigami.Heading.Type.Primary
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                QQC2.ComboBox {
                    Layout.fillWidth: true
                    id: roleCombobox
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    model: [
                        ///todo: Dynamically fetch the roles
                        {
                            display: i18nc("@info:Filter out accounts with any role", "All"),
                            value: ""
                        },
                        {
                            display: i18nc("@info:Filter out accounts with moderator role", "Moderator"),
                            value: "1"
                        },
                        {
                            display: i18nc("@info:Filter out accounts with admin role", "Admin"),
                            value: "2"
                        },
                        {
                            display: i18nc("@info:Filter out accounts with owner role", "Owner"),
                            value: "3"
                        },
                    ]
                    textRole: "display"
                    valueRole: "value"
                    Component.onCompleted: roleCombobox.currentIndex = roleCombobox.indexOfValue(accountView.model.role);
                    onCurrentIndexChanged: accountView.model.role = model[currentIndex].value
                }
            }
        }
        QQC2.Button {
            icon.name: "search"
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            text: i18n("Advanced Search")
            Layout.alignment: Qt.AlignHCenter
            onClicked: textPromptDialog.open()
        }
        Kirigami.Separator {
            Layout.fillWidth: true
        }
    }

    ListView {
        id: accountView
        model: AccountsToolModel{}

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property int index
            required property var identity

            implicitWidth: ListView.view.width
            Layout.fillWidth: true

            onClicked: root.QQC2.ApplicationWindow.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "MainAccountToolPage"), {
                identity: delegate.identity,
                index: delegate.index,
                model: accountView.model
            })

            contentItem: Kirigami.FlexColumn {
                spacing: 0
                RowLayout {
                    spacing: 0
                    Layout.fillWidth: true
                    InlineIdentityInfo {
                        identity: delegate.identity.userLevelIdentity
                        secondary: false
                        admin: true
                        ip: delegate.identity.ip
                    }
                    ColumnLayout {
                        spacing: 0
                        Kirigami.Heading {
                            level: 4
                            Layout.alignment: Qt.AlignRight
                            text: delegate.identity.userLevelIdentity.statusesCount + " Posts"
                            type: Kirigami.Heading.Type.Secondary
                            elide: Text.ElideRight
                        }
                        Kirigami.Heading {
                            level: 4
                            Layout.alignment: Qt.AlignRight
                            text: delegate.identity.userLevelIdentity.followersCount + " Followers"
                            type: Kirigami.Heading.Type.Secondary
                            elide: Text.ElideRight
                        }
                        Kirigami.Heading {
                            id: emailHeading
                            level: 4
                            Layout.alignment: Qt.AlignRight
                            text: delegate.identity.emailProvider
                            visible: delegate.identity.emailProvider
                            type: Kirigami.Heading.Type.Secondary
                            elide: Text.ElideRight
                        }
                    }
                }
                QQC2.ProgressBar {
                    visible: accountView.model.loading && (index == accountView.count - 1)
                    indeterminate: true
                    padding: Kirigami.Units.largeSpacing * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                }
            }
        }
        QQC2.ProgressBar {
            visible: accountView.model.loading && accountView.count === 0
            anchors.centerIn: parent
            indeterminate: true
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No accounts found")
            visible: accountView.count === 0 && !accountView.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
