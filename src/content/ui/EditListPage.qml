// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as Components

import org.kde.tokodon

FormCard.FormCardPage {
    id: root

    enum Purpose {
        New,
        Edit
    }

    property var purpose
    property string listId

    readonly property bool isValid: titleField.text.length > 0

    title: purpose === EditListPage.New ? i18nc("@title:window", "Create List") : i18nc("@title:window", "Edit List")

    property ListEditorBackend backend: ListEditorBackend {
        listId: root.listId
        title: titleField.text
        exclusive: exclusiveField.checked
    }

    data: Connections {
        target: backend

        function onLoadingChanged() {
            // If we loaded data, then overwrite the fields
            titleField.text = backend.title;
            exclusiveField.checked = backend.exclusive;
            repliesPolicyField.currentIndex = backend.replyPolicyIndex();
        }

        function onDone() {
            pageStack.layers.pop();
        }
    }

    Component.onCompleted: titleField.forceActiveFocus()

    FormCard.FormCard {
        enabled: !backend.loading

        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormTextFieldDelegate {
            id: titleField
            label: i18nc("@label:textbox List title", "Title")
            onAccepted: exclusiveField.forceActiveFocus()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            id: repliesPolicyField
            text: i18nc("@label", "Show replies for")
            model: backend.replyPolicies()
            currentIndex: 0
            onCurrentIndexChanged: backend.setReplyPolicyIndex(currentIndex)

            Layout.fillWidth: true
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormCheckDelegate {
            id: exclusiveField
            text: i18nc("@label If the list is exclusive", "Exclusive")
            description: i18n("Posts in an exclusive list are excluded from the Home timeline.")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: manageUsersButton
            visible: root.purpose === EditListPage.Edit
            text: i18nc("@action:button Delete the list", "Manage Users")
            onClicked: pageStack.layers.push(socialGraphComponent, { name: "list", listId: root.listId });
        }
    }

    Component {
        id: socialGraphComponent
        SocialGraphPage {
            id: socialGraphPage
            property alias name: socialGraphModel.name
            property alias listId: socialGraphModel.listId
            model: SocialGraphModel {
                id: socialGraphModel
            }
        }
    }

    FormCard.FormCard {
        enabled: !backend.loading

        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormButtonDelegate {
            id: createButton
            enabled: root.isValid
            icon.name: {
                if (root.purpose === EditListPage.New) {
                    return "gtk-add";
                } else {
                    return "edit-rename";
                }
            }
            text: {
                if (root.purpose === EditListPage.New) {
                    return i18nc("@action:button Create the list", "Create");
                } else {
                    return i18nc("@action:button Edit the list", "Edit");
                }
            }
            onClicked: backend.submit()
        }

        FormCard.FormDelegateSeparator {
            visible: deleteButton.visible
        }

        FormCard.FormButtonDelegate {
            id: deleteButton
            visible: root.purpose === EditListPage.Edit
            text: i18nc("@action:button Delete the list", "Delete")
            icon.name: "edit-delete"
            onClicked: removeListPrompt.open()

            Kirigami.PromptDialog {
                id: removeListPrompt

                title: i18nc("@title", "Deleting List")
                subtitle: i18nc("@label", "Are you sure you want to delete this list?")
                standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
                showCloseButton: false

                onAccepted: backend.deleteList()
            }
        }
    }
}
