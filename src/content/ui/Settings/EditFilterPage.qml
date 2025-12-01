// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
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

    property int purpose
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

        function onLoadingChanged(): void {
            // If we loaded data, then overwrite the fields
            titleField.text = backend.title;
            homeAndListsDelegate.checked = backend.homeAndListsContext;
            notificationsDelegate.checked = backend.notificationsContext;
            publicTimelinesDelegate.checked = backend.publicTimelinesContext;
            conversationsDelegate.checked = backend.conversationsContext;
            profilesDelegate.checked = backend.profilesContext;


            hideWithNoticeDelegate.checked = false;
            hideMediaOnlyDelegate.checked = false;
            hideCompletelyDelegate.checked = false;

            if (backend.filterAction === "warn") {
                hideWithNoticeDelegate.checked = true;
            } else if (backend.filterAction === "blur") {
                hideMediaOnlyDelegate.checked = true;
            } else if (backend.filterAction === "hide") {
                hideCompletelyDelegate.checked = true;
            }
        }

        function onDone(): void {
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
    }

    FormCard.FormHeader {
        title: i18nc("@info:header", "Contexts")
    }

    FormCard.FormCard {
        enabled: !backend.loading

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
    }

    FormCard.FormHeader {
        title: i18nc("@info:header", "Action")
    }

    FormCard.FormCard {
        enabled: !backend.loading

        FormCard.FormRadioDelegate {
            id: hideWithNoticeDelegate
            text: i18nc("@label:radiobutton", "Hide with Content Notice")
            onToggled: backend.filterAction = "warn"
        }

        FormCard.FormRadioDelegate {
            id: hideMediaOnlyDelegate
            text: i18nc("@label:radiobutton", "Hide Media with Content Notice")
            visible: AccountManager.selectedAccount.supportsApiVersion("mastodon", 5)
            onToggled: backend.filterAction = "blur"
        }

        FormCard.FormRadioDelegate {
            id: hideCompletelyDelegate
            text: i18nc("@label:radiobutton", "Hide Completely")
            onToggled: backend.filterAction = "hide"
        }
    }

    FormCard.FormHeader {
        title: i18nc("@info:header", "Keywords")

        actions: QQC2.Action {
            text: i18nc("@action:button Add keyword", "Add")
            icon.name: "list-add-symbolic"
            onTriggered: backend.addKeyword()
        }
    }

    FormCard.FormCard {
        enabled: !backend.loading

        FormCard.FormPlaceholderMessageDelegate {
            text: i18nc("@info:placeholder", "No keywords")
            visible: keywordsRepeater.count === 0
        }

        Repeater {
            id: keywordsRepeater

            model: backend.keywords

            FormCard.AbstractFormDelegate {
                id: delegate

                required property int index
                required property string keyword
                required property bool whole_word

                background: null
                contentItem: RowLayout {
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
                    return "document-save-symbolic";
                }
            }
            text: {
                if (root.purpose === EditFilterPage.New) {
                    return i18nc("@action:button Create the filter", "Create");
                } else {
                    return i18nc("@action:button Save the edited filter", "Save");
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
