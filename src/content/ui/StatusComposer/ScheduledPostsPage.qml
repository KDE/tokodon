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
        id: listView

        model: ScheduledStatusesModel {
            id: model
            drafts: root.drafts
        }

        delegate: MinimalPostDelegate {
            id: delegate

            required property string scheduledAt

            authorIdentity: null

            notificationActorIdentity: null

            spoilerText: ""
            relativeTime: ""
            poll: null
            selected: false
            filters: []
            sensitive: false
            type: 0

            post: null

            isGroup: false
            numInGroup: 0
            standaloneTags: []

            width: ListView.view.width

            function clicked(): bool {
                // You can only really "open" draft posts
                if (root.drafts) {
                    root.opened(id);
                }
                return true;
            }

            Kirigami.Heading {
                level: 4
                text: root.drafts ? i18nc("Draft, unfinished post", "Created %1 ago", delegate.scheduledAt) : i18nc("Scheduled for this date", "Scheduled for %1", delegate.scheduledAt)
            }

            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                text: i18nc("@action:button Discard this post", "Discard…")
                icon.name: "delete-symbolic"
                onClicked: {
                    root.deletedRow = delegate.index;
                    discardDraftPrompt.open();
                }
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "view-list-text"
            text: root.drafts ? i18nc("@info:placeholder", "No Drafts") : i18nc("@info:placeholder", "No Scheduled Posts")
            visible: listView.count === 0 && !listView.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }

    Kirigami.PromptDialog {
        id: discardDraftPrompt

        title: root.drafts ? i18nc("@title", "Discard Draft") : i18nc("@title", "Discard Scheduled Post")
        subtitle: root.drafts ? i18nc("@label", "Are you sure you want to discard this draft?") : i18nc("@label", "Are you sure you want to discard this scheduled post?")
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
