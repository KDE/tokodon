// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigamiaddons.delegates as Delegates
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
    property bool deleted

    readonly property bool isValid: titleField.text.length > 0

    title: purpose === EditListPage.New ? i18nc("@title:window", "Create List") : i18nc("@title:window", "Edit List")

    property ListEditorBackend backend: ListEditorBackend {
        listId: root.listId
        title: titleField.text
        exclusive: exclusiveField.checked
        favorite: favoriteField.checked
    }

    signal done(bool deleted)

    data: Connections {
        target: backend

        function onLoadingChanged() {
            // If we loaded data, then overwrite the fields
            titleField.text = backend.title;
            exclusiveField.checked = backend.exclusive;
            favoriteField.checked = backend.favorite;
            repliesPolicyField.currentIndex = backend.replyPolicyIndex();
        }

        function onDone() {
            root.done(root.deleted);
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

        FormCard.FormDelegateSeparator {
            visible: purpose !== EditListPage.New
        }

        FormCard.FormCheckDelegate {
            id: favoriteField
            visible: purpose !== EditListPage.New
            text: i18nc("@label If the list is favorited", "Favorite")
            description: i18n("This list will show up in the sidebar.")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            id: manageUsersButton
            visible: root.purpose === EditListPage.Edit
            text: i18nc("@action:button Delete the list", "Manage Users")
            onClicked: pageStack.layers.push(socialGraphComponent, { name: "list", listId: root.listId })
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

            actions: [
                Kirigami.Action {
                    text: i18nc("@action:intoolbar Add users to list", "Add Users")
                    icon.name: "list-add-user"
                    onTriggered: pageStack.layers.push(addUserComponent, { socialGraphModel })
                }
            ]

            Component {
                id: addUserComponent
                Kirigami.ScrollablePage {
                    id: addUserPage

                    title: i18nc("@title", "Add Users to List…")

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
                                        text: i18nc("@action:button Add user to this list", "Add")
                                        icon.name: "checkmark"
                                        onClicked: socialGraphModel.actionAddToList(delegate.authorIdentity.id)
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

                onAccepted: {
                    root.deleted = true;
                    backend.deleteList();
                }
            }
        }
    }
}
