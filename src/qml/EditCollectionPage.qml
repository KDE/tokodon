// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as Components

import org.kde.tokodon

FormCard.FormCardPage {
    id: root

    enum Purpose {
        New,
        Edit
    }

    property var purpose
    property string collectionId
    property bool deleted

    readonly property bool isValid: nameField.text.length > 0

    title: purpose === EditListPage.New ? i18nc("@title:window", "Create Collection") : i18nc("@title:window", "Edit Collection")

    property CollectionEditorBackend backend: CollectionEditorBackend {
        collectionId: root.collectionId
        name: nameField.text
        description: descriptionField.text
        sensitive: sensitiveField.checked
        discoverable: discoverableField.checked
    }

    signal done(bool deleted)

    data: Connections {
        target: backend

        function onLoadingChanged(): void {
            // If we loaded data, then overwrite the fields
            nameField.text = backend.name;
            descriptionField.text = backend.description;
            sensitiveField.checked = backend.sensitive;
            discoverableField.checked = backend.discoverable;
        }

        function onDone(): void {
            root.done(root.deleted);
        }
    }

    Component.onCompleted: nameField.forceActiveFocus()

    FormCard.FormCard {
        enabled: !backend.loading

        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormTextFieldDelegate {
            id: nameField
            label: i18nc("@label:textbox Collection title", "Name")
        }

        FormCard.FormDelegateSeparator {
            above: nameField
            below: descriptionField
        }

        FormCard.FormTextFieldDelegate {
            id: descriptionField
            label: i18nc("@label:textbox Collection description", "Description")
        }

        FormCard.FormDelegateSeparator {
            above: descriptionField
            below: sensitiveField
        }

        FormCard.FormCheckDelegate {
            id: sensitiveField
            text: i18nc("@label:textbox Whether this collection should be marked sensitive", "Sensitive")
        }

        FormCard.FormDelegateSeparator {
            above: sensitiveField
            below: discoverableField
        }

        FormCard.FormCheckDelegate {
            id: discoverableField
            text: i18nc("@label:textbox Whether this collection should be discoverable by other users", "Discoverable")
        }

        FormCard.FormDelegateSeparator {
            above: discoverableField
            below: manageUsersButton
        }

        FormCard.FormButtonDelegate {
            id: manageUsersButton
            visible: root.purpose === EditCollectionPage.Edit
            text: i18nc("@action:intoolbar Add users to collection", "Add Users")
            icon.name: "list-add-user"
            onClicked: pageStack.layers.push(addUserComponent, { socialGraphModel })
        }
    }

    Component {
        id: addUserComponent
        Kirigami.ScrollablePage {
            id: addUserPage

            title: i18nc("@title", "Add Users to Collection…")

            required property SocialGraphModel socialGraphModel

            ListView {
                id: listView

                model: SearchModel {
                    id: searchModel
                }

                delegate: Delegates.RoundedItemDelegate {
                    id: delegate

                    required property var index
                    required property var authorIdentity

                    text: authorIdentity.displayName

                    onClicked: Navigation.openAccount(delegate.authorIdentity.id)

                    contentItem: ColumnLayout {
                        spacing: 0

                        RowLayout {
                            Layout.fillWidth: true

                            spacing: 0

                            InlineIdentityInfo {
                                identity: delegate.authorIdentity
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            QQC2.Button {
                                text: i18nc("@action:button Add user to this collection", "Add")
                                icon.name: "checkmark"
                                onClicked: socialGraphModel.actionAddToCollection(delegate.authorIdentity.id)
                            }
                        }

                        QQC2.ProgressBar {
                            visible: delegate.ListView.view.model.loading && (index == listview.count - 1)
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

                header: QQC2.Control {
                    topPadding: 0
                    bottomPadding: 0
                    leftPadding: 0
                    rightPadding: 0

                    width: listView.width

                    background: Rectangle {
                        Kirigami.Theme.colorSet: Kirigami.Theme.Window
                        Kirigami.Theme.inherit: false
                        color: Kirigami.Theme.backgroundColor

                        Kirigami.Separator {
                            anchors {
                                left: parent.left
                                bottom: parent.bottom
                                right: parent.right
                            }
                        }
                    }

                    contentItem: ColumnLayout {
                        spacing: 0

                        Kirigami.SearchField {
                            id: searchField
                            placeholderText: i18n("Search among users you follow…")

                            Layout.fillWidth: true
                            Layout.margins: Kirigami.Units.largeSpacing

                            onAccepted: if (text.length > 2) {
                                listView.model.search(text, "accounts", true)
                            }
                        }
                    }
                }
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
                if (root.purpose === EditCollectionPage.New) {
                    return "gtk-add";
                } else {
                    return "document-save-symbolic";
                }
            }
            text: {
                if (root.purpose === EditCollectionPage.New) {
                    return i18nc("@action:button Create the collection", "Create");
                } else {
                    return i18nc("@action:button Save the edited collection", "Save");
                }
            }
            onClicked: backend.submit()
        }

        FormCard.FormDelegateSeparator {
            visible: deleteButton.visible
        }

        FormCard.FormButtonDelegate {
            id: deleteButton
            visible: root.purpose === EditCollectionPage.Edit
            text: i18nc("@action:button Delete the collection", "Delete")
            icon.name: "edit-delete"
            onClicked: removeCollectionPrompt.open()

            Kirigami.PromptDialog {
                id: removeCollectionPrompt

                title: i18nc("@title", "Deleting Collection")
                subtitle: i18nc("@label", "Are you sure you want to delete this collection?")
                standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
                showCloseButton: false

                onAccepted: {
                    root.deleted = true;
                    backend.deleteCollection();
                }
            }
        }
    }
}
