// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.tokodon

import "../PostDelegate" as PostDelegate

Kirigami.ScrollablePage {
    id: root

    // Whether to display draft or scheduled posts
    required property bool drafts

    // To make sure we're gone once the post is loaded
    property PostEditorBackend backend

    // Current row being interacted with (e.g. deletion)
    property int currentRow

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
            required property date scheduledAtDate

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
                text: i18nc("@action:button Re-schedule this post", "Re-schedule…")
                icon.name: "resource-calendar-insert"
                visible: !root.drafts
                onClicked: {
                    root.currentRow = delegate.index;
                    schedulePostPrompt.value = delegate.scheduledAtDate;
                    schedulePostPrompt.open();
                }
            }

            QQC2.Button {
                text: i18nc("@action:button Discard this post", "Discard…")
                icon.name: "delete-symbolic"
                onClicked: {
                    root.currentRow = delegate.index;
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
                    model.deleteDraft(model.index(root.currentRow, 0));
                    discardDraftPrompt.close();
                }
            }
        ]
    }


    Kirigami.Dialog {
        id: schedulePostPrompt

        property alias value: scheduleDateTimeDelegate.value

        title: i18nc("@title", "Re-Schedule Post")
        standardButtons: Kirigami.Dialog.Cancel
        showCloseButton: false
        implicitWidth: 300

        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@action:button Set this post's schedule to when it should be posted", "Set New Schedule")
                icon.name: "resource-calendar-insert"
                onTriggered: {
                    model.reschedule(model.index(root.currentRow, 0), schedulePostPrompt.value);
                    schedulePostPrompt.close();
                }
            }
        ]

        ColumnLayout {
            spacing: 0

            FormCard.FormCard {
                FormCard.FormDateTimeDelegate {
                    id: scheduleDateTimeDelegate
                }
            }
        }
    }
}
