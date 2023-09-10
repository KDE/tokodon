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

    actions: Kirigami.Action {
        text: i18nc("@action:button", "Create Email Block")
        icon.name: 'list-add'
        onTriggered: newEmailBlockDialog.open()
    }

    Kirigami.PromptDialog {
        id: emailInfoDialog

        property int index
        property string domain
        property int accountCount
        property date createdAt
        property int ipCount

        title: i18nc("@title", "E-Mail Domain Info")

        contentPadding: 0
        implicitWidth: Kirigami.Units.gridUnit * 20

        mainItem: ColumnLayout {
            spacing: 0

            FormCard.FormTextDelegate {
                text: i18nc("@info The domain on which the block is imposed", "Email domain name")
                description: emailInfoDialog.domain
                Layout.fillWidth: true
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormTextDelegate {
                text: i18nc("@info Time when the block was imposed.", "Block created at")
                description: emailInfoDialog.createdAt.toDateString()
                Layout.fillWidth: true
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormTextDelegate {
                text: i18nc("@info The counted accounts signup attempts using that email domain within the last week.", "Account sign-up attempts in this week")
                description: emailInfoDialog.accountCount
                Layout.fillWidth: true
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormTextDelegate {
                text: i18nc("@info The counted IP signup attempts of that email domain within that day.", "IP sign-up attempts in this week")
                description: emailInfoDialog.ipCount
                Layout.fillWidth: true
            }
        }

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@action:button", "Cancel")
                icon.name: "dialog-cancel"
                onTriggered: emailInfoDialog.close();
            },
            Kirigami.Action {
                text: i18nc("@action:button", "Delete email block")
                icon.name: "delete"
                onTriggered: {
                    emailBlockView.model.deleteEmailBlock(emailInfoDialog.index)
                    showPassiveNotification(i18n("Email block deleted"))
                    emailInfoDialog.close();
                }
            }
        ]
    }

    Kirigami.PromptDialog {
        id: newEmailBlockDialog

        title: i18nc("@title", "New E-Mail Domain Block")

        contentPadding: 0
        implicitWidth: Kirigami.Units.gridUnit * 20

        mainItem: ColumnLayout {
            spacing: 0

            FormCard.FormTextFieldDelegate {
                id: email
                label: i18nc("@info The domain on which the block will be imposed", "Domain *")
                statusMessage: i18n("This can be the domain name that shows up in the e-mail address or the MX record it uses. They will be checked upon sign-up.")
                status: Kirigami.MessageType.Information
            }
        }

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@action:button", "Cancel")
                icon.name: "dialog-cancel"
                onTriggered: newEmailBlockDialog.close();
            },
            Kirigami.Action {
                text: i18nc("@action:button", "Resolve domain")
                icon.name: "checkbox"
                onTriggered: {
                    emailBlockView.model.newEmailBlock(email.text)
                    showPassiveNotification(i18n("New email block added"))
                    email.clear()
                    newEmailBlockDialog.close();
                }
            }
        ]
    }

    ListView {
        id: emailBlockView

        model: EmailBlockToolModel {}
        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property int index
            required property int id
            required property string domain
            required property date createdAt
            required property int accountCount
            required property int ipCount

            property int totalCount: delegate.ipCount + delegate.accountCount

            width: ListView.view.width

            text: delegate.domain

            onClicked: {
                emailInfoDialog.index = delegate.index
                emailInfoDialog.domain = delegate.domain
                emailInfoDialog.createdAt = delegate.createdAt
                emailInfoDialog.ipCount = delegate.ipCount
                emailInfoDialog.accountCount = delegate.accountCount
                emailInfoDialog.open()
            }

            contentItem: RowLayout {
                Delegates.SubtitleContentItem {
                    itemDelegate: delegate
                    bold: true
                    subtitle: i18nc("@info", "%1 sign-up attempts over the last week", delegate.totalCount)
                }

                Item {
                    Layout.fillWidth: true
                }

                Kirigami.Heading {
                    level: 3
                    text: delegate.createdAt.toLocaleDateString()
                    type: Kirigami.Heading.Type.Secondary
                    Layout.alignment: Qt.AlignRight
                }
            }

            QQC2.ProgressBar {
                visible: emailBlockView.model.loading && emailBlockView.count === 0
                anchors.centerIn: parent
                indeterminate: true
            }
            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent
                text: i18n("No email blocks found")
                visible: emailBlockView.count === 0 && !emailBlockView.model.loading
                width: parent.width - Kirigami.Units.gridUnit * 4
            }
        }
    }
}
