// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
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
    property string filterId
    property bool deleted

    readonly property bool isValid: titleField.text.length > 0

    title: purpose === EditFilterPage.New ? i18nc("@title:window", "Create Filter") : i18nc("@title:window", "Edit Filter")

    property FilterEditorBackend backend: FilterEditorBackend {
        filterId: root.filterId
        title: titleField.text
        homeAndListsContext: homeAndListsDelegate.checked
        notificationsContext: notificationsDelegate.checked
        publicTimelinesContext: publicTimelinesDelegate.checked
        conversationsContext: conversationsDelegate.checked
        profilesContext: profilesDelegate.checked
    }

    signal done(bool deleted)

    data: Connections {
        target: backend

        function onLoadingChanged() {
            // If we loaded data, then overwrite the fields
            titleField.text = backend.title;
            homeAndListsDelegate.checked = backend.homeAndListsContext;
            notificationsDelegate.checked = backend.notificationsContext;
            publicTimelinesDelegate.checked = backend.publicTimelinesContext;
            conversationsDelegate.checked = backend.conversationsContext;
            profilesDelegate.checked = backend.profilesContext;
            hideWithNoticeDelegate.checked = !backend.hideCompletely;
            hideCompletelyDelegate.checked = backend.hideCompletely;
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
            label: i18nc("@label:textbox Filter title", "Title")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@info", "Select one or more contexts where this filter should apply:")
        }

        FormCard.FormCheckDelegate {
            id: homeAndListsDelegate
            text: i18nc("@label:checkbox", "Home and Lists")
        }

        FormCard.FormCheckDelegate {
            id: notificationsDelegate
            text: i18nc("@label:checkbox", "Notifications")
        }

        FormCard.FormCheckDelegate {
            id: publicTimelinesDelegate
            text: i18nc("@label:checkbox", "Public Timelines")
        }

        FormCard.FormCheckDelegate {
            id: conversationsDelegate
            text: i18nc("@label:checkbox", "Conversations")
        }

        FormCard.FormCheckDelegate {
            id: profilesDelegate
            text: i18nc("@label:checkbox", "Profiles")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormRadioDelegate {
            id: hideWithNoticeDelegate
            text: i18nc("@label:radiobutton", "Hide with Content Notice")
            onToggled: backend.hideCompletely = false
        }

        FormCard.FormRadioDelegate {
            id: hideCompletelyDelegate
            text: i18nc("@label:radiobutton", "Hide Completely")
            onToggled: backend.hideCompletely = true
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@info", "Keywords")
        }

        FormCard.AbstractFormDelegate {
            contentItem: ColumnLayout {
                spacing: Kirigami.Units.smallSpacing

                Repeater {
                    model: backend.keywords

                    delegate: RowLayout {
                        id: delegate

                        required property int index
                        required property string keyword
                        required property bool whole_word

                        spacing: Kirigami.Units.smallSpacing

                        QQC2.TextField {
                            text: delegate.keyword
                            onEditingFinished: backend.editKeyword(delegate.index, text)

                            Layout.fillWidth: true
                        }

                        QQC2.CheckBox {
                            text: i18nc("@label:checkbox", "Whole word")
                            checked: delegate.whole_word
                            onToggled: backend.editWholeWord(delegate.index, checked)
                        }

                        QQC2.Button {
                            text: i18nc("@action:button Remove keyword", "Remove")
                            icon.name: "list-remove-symbolic"
                            display: QQC2.AbstractButton.IconOnly
                            onClicked: backend.removeKeyword(delegate.index)
                        }
                    }
                }

                QQC2.Button {
                    text: i18nc("@action:button Add keyword", "Add")
                    icon.name: "list-add-symbolic"
                    onClicked: backend.addKeyword()
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
                if (root.purpose === EditFilterPage.New) {
                    return "gtk-add";
                } else {
                    return "edit-rename";
                }
            }
            text: {
                if (root.purpose === EditFilterPage.New) {
                    return i18nc("@action:button Create the filter", "Create");
                } else {
                    return i18nc("@action:button Edit the filter", "Edit");
                }
            }
            onClicked: backend.submit()
        }
    
        FormCard.FormDelegateSeparator {
            visible: deleteButton.visible
        }
    
        FormCard.FormButtonDelegate {
            id: deleteButton
            visible: root.purpose === EditFilterPage.Edit
            text: i18nc("@action:button Delete the filter", "Delete")
            icon.name: "edit-delete"
            onClicked: removeFilterPrompt.open()
    
            Kirigami.PromptDialog {
                id: removeFilterPrompt
    
                title: i18nc("@title", "Deleting Filter")
                subtitle: i18nc("@label", "Are you sure you want to delete this filter?")
                standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
                showCloseButton: false

                onAccepted: {
                    root.deleted = true;
                    backend.deleteFilter();
                }
            }
        }
    }
}
