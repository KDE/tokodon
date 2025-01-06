// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigamiaddons.delegates 1 as Delegates
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import org.kde.tokodon

/**
 * Other notification posts from other users, e.g. replies.
 */
NotificationDelegate {
    id: root

    required property string id
    required property string content
    required property var authorIdentity
    required property string relativeTime
    required property var timelineModel
    required property bool isGroup
    required property bool isInGroup
    required property int numInGroup
    required property var notificationActorIdentity
    required property int type

    ColumnLayout {
        id: rowLayout

        spacing: Kirigami.Units.smallSpacing

        // interaction label
        RowLayout {
            spacing: 0

            Loader {
                active: root.isGroup

                sourceComponent: GroupInteractionLabel {
                    type: root.type
                    notificationActorIdentity: root.notificationActorIdentity
                    numInGroup: root.numInGroup
                }
            }

            Loader {
                active: !root.isGroup

                sourceComponent: UserInteractionLabel {
                    type: root.type
                    notificationActorIdentity: root.notificationActorIdentity
                }
            }

            Item {
                Layout.fillWidth: true
            }

            QQC2.Label {
                text: root.relativeTime
                color: Kirigami.Theme.disabledTextColor
            }
        }

        PostInfoBar {
            id: infoBar

            identity: root.authorIdentity
            secondary: root.secondary
            visible: !root.filtered
            relativeTime: root.relativeTime
            selected: root.selected
            wasEdited: root.wasEdited
            editedAt: root.editedAt
            visibility: root.visibility
            absoluteTime: root.absoluteTime

            Layout.fillWidth: true

            onMoreOpened: parentItem => {
                parentItem.down = true;
                const item = flexColumn.postMenu.createObject(QQC2.ApplicationWindow.window);
                item.closed.connect(() => parentItem.down = false);
                item.popup();
            }

            Loader {
                id: deleteDialog

                active: false
                visible: false // to prevent the menu from taking space in the layout

                sourceComponent: Kirigami.PromptDialog {
                    title: i18nc("@title", "Delete Post")
                    subtitle: i18nc("@label", "Are you sure you want to delete this post?")
                    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
                    showCloseButton: false

                    onAccepted: root.timelineModel.actionDelete(timelineModel.index(root.index, 0))
                }
            }

            Loader {
                id: redraftDialog

                active: false
                visible: false

                sourceComponent: Kirigami.PromptDialog {
                    title: i18nc("@title", "Delete & Re-draft Post")
                    subtitle: i18nc("@label", "Are you sure you want to redraft this post? This will delete the original post.")
                    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
                    showCloseButton: false

                    onAccepted: root.timelineModel.actionRedraft(timelineModel.index(root.index, 0), false)
                }
            }
        }

        PostContent {
            content: root.content
            expandedPost: false
            secondary: false
            shouldOpenInternalLinks: true
        }
    }
}
