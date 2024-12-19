// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.tokodon

import "../PostDelegate" as PostDelegate

Kirigami.ScrollablePage {
    id: root

    // Whether to display draft or scheduled posts
    required property bool drafts

    // To make sure we're gone once the post is loaded
    property PostEditorBackend backend

    property int deletedRow

    title: model.displayName

    signal opened(id: string)

    Connections {
        target: backend

        function onScheduledPostLoaded(): void {
            pageStack.layers.pop();
        }
    }

    ListView {
        model: ScheduledStatusesModel {
            id: model
            drafts: root.drafts
        }

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property string id
            required property string scheduledAt
            required text
            required property int index

            contentItem: RowLayout {
                spacing: Kirigami.Units.largeSpacing

                ColumnLayout {
                    spacing: 0

                    Layout.fillHeight: true

                    Kirigami.Heading {
                        level: 4
                        text: root.drafts ? i18nc("Draft, unfinished post", "Draft") : i18nc("Scheduled for this date", "Scheduled for %1", delegate.scheduledAt)
                    }

                    PostDelegate.PostContent {
                        content: delegate.text
                        expandedPost: false
                        secondary: true
                        shouldOpenInternalLinks: false
                        hoverEnabled: false

                        onClicked: delegate.click()

                        Layout.fillWidth: true
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                QQC2.Button {
                    text: i18nc("@action:button Discard this post", "Discard")
                    icon.name: "delete-symbolic"
                    onClicked: {
                        root.deletedRow = delegate.index;
                        discardDraftPrompt.open();
                    }
                }

                QQC2.Label {
                    text: delegate.scheduledAt
                    visible: root.drafts
                    color: Kirigami.Theme.disabledTextColor
                }
            }

            onClicked: root.opened(id)
        }
    }

    Kirigami.PromptDialog {
        id: discardDraftPrompt

        title: i18nc("@title", "Discard Draft")
        subtitle: i18nc("@label", "Are you sure you want to discard your draft?")
        standardButtons: Kirigami.Dialog.Cancel
        showCloseButton: false

        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@action:button Discard this draft", "Discard")
                icon.name: "delete-symbolic"
                onTriggered: {
                    model.deleteDraft(model.index(root.deletedRow, 0));
                    discardDraftPrompt.close();
                }
            }
        ]
    }
}
