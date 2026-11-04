// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.delegates as Delegates

Kirigami.ScrollablePage {
    id: root

    actions: Kirigami.Action {
        text: KI18n.i18nc("@action:button", "Create Email Block")
        icon.name: 'list-add'
        onTriggered: newEmailBlockDialog.open()
    }

    FormCard.FormCardDialog {
        id: emailInfoDialog
        parent: root.QQC2.Overlay.overlay

        property int index
        property string domain
        property int accountCount
        property date createdAt
        property int ipCount

        title: KI18n.i18nc("@title", "E-Mail Domain Info")
        autoSeparators: true

        standardButtons: FormCard.FormCardDialog.Ok | FormCard.FormCardDialog.Cancel

        Component.onCompleted: {
            const deleteButton = standardButton(FormCard.FormCardDialog.Ok);
            deleteButton.text = KI18n.i18nc("@action:button", "Delete email block");
            deleteButton.icon.name = "delete";
        }

        onAccepted: {
            emailBlockView.model.deleteEmailBlock(emailInfoDialog.index)
            root.QQC2.ApplicationWindow.window.showPassiveNotification(KI18n.i18n("Email block deleted"))
            emailInfoDialog.close();
        }

        onRejected: emailInfoDialog.close();

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@info The domain on which the block is imposed", "Email domain name")
            description: emailInfoDialog.domain
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@info Time when the block was imposed.", "Block created at")
            description: emailInfoDialog.createdAt.toDateString()
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@info The counted accounts signup attempts using that email domain within the last week.", "Account sign-up attempts in this week")
            description: emailInfoDialog.accountCount
        }

        FormCard.FormTextDelegate {
            text: KI18n.i18nc("@info The counted IP signup attempts of that email domain within that day.", "IP sign-up attempts in this week")
            description: emailInfoDialog.ipCount
        }
    }

    FormCard.FormCardDialog {
        id: newEmailBlockDialog
        parent: root.QQC2.Overlay.overlay

        title: KI18n.i18nc("@title", "New E-Mail Domain Block")
        autoSeparators: true

        standardButtons: FormCard.FormCardDialog.Ok | FormCard.FormCardDialog.Cancel

        Component.onCompleted: {
            const createButton = standardButton(FormCard.FormCardDialog.Ok);
            createButton.text = KI18n.i18nc("@action:button", "Resolve domain");
            createButton.icon.name = "checkbox";
        }

        onAccepted: {
            emailBlockView.model.newEmailBlock(email.text)
            root.QQC2.ApplicationWindow.window.showPassiveNotification(KI18n.i18n("New email block added"))
            email.clear()
            newEmailBlockDialog.close();
        }

        onRejected: newEmailBlockDialog.close();

        FormCard.FormTextFieldDelegate {
            id: email
            label: KI18n.i18nc("@info The domain on which the block will be imposed", "Domain *")
            statusMessage: KI18n.i18n("This can be the domain name that shows up in the e-mail address or the MX record it uses. They will be checked upon sign-up.")
            status: Kirigami.MessageType.Information
        }
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
                    subtitle: KI18n.i18nc("@info", "%1 sign-up attempts over the last week", delegate.totalCount)
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
                text: KI18n.i18n("No email blocks found")
                visible: emailBlockView.count === 0 && !emailBlockView.model.loading
                width: parent.width - Kirigami.Units.gridUnit * 4
            }
        }
    }
}
