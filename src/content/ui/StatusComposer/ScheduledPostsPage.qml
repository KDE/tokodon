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

            contentItem: ColumnLayout {
                spacing: Kirigami.Units.smallSpacing

                RowLayout {
                    Layout.fillWidth: true

                    Kirigami.Heading {
                        level: 4
                        text: root.drafts ? i18nc("Draft, unfinished post", "Draft") : i18nc("Scheduled for this date", "Scheduled for %1", delegate.scheduledAt)
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    QQC2.Label {
                        text: delegate.scheduledAt
                        visible: root.drafts
                    }
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

            onClicked: root.opened(id)
        }
    }
}
