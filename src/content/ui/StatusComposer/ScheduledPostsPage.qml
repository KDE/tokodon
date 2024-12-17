// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.tokodon

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
            required property var scheduledAt
            required text

            contentItem: Kirigami.TitleSubtitle {
                title: root.drafts ? i18nc("Draft, unfinished post", "Draft") : i18nc("Scheduled for this date", "Scheduled for %1", delegate.scheduledAt)
                subtitle: delegate.text
                font: delegate.font
                selected: delegate.highlighted || delegate.down
            }

            onClicked: root.opened(id)
        }
    }
}
