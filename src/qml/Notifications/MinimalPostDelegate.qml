// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.tokodon


import "../Notifications" as Notifications

/**
 * @brief A smaller post delegate meant for showing displaying secondary content. A good example is a reply preview, or a notification about your own post.
 */
QQC2.ItemDelegate {
    id: root

    default property alias children: extraItemsLayout.children

    // The model roles
    required property int index

    required property string id
    required property var authorIdentity

    required property var notificationActorIdentity

    required property string content
    required property string spoilerText
    required property string relativeTime
    required property var attachments
    required property var poll
    required property bool selected
    required property var filters
    required property bool sensitive
    required property int type
    required property bool hasContent
    required property var standaloneTags

    required property var post

    required property bool isGroup
    required property int numInGroup

    property bool filtered: root.filters.length > 0
    property var timelineModel
    property bool showSeparator: true
    property bool loading: false
    property bool inViewPort: true

    padding: 0
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    leftPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing

    topInset: 0
    leftInset: 0
    rightInset: 0
    bottomInset: 0

    highlighted: false
    hoverEnabled: false

    Accessible.description: root.spoilerText.length === 0 ? i18n("Normal Status") : i18n("Spoiler Status")

    function openPost(): void {
        if (root.clicked()) {
            return;
        }
        if (postContent.hoveredLink) {
            return;
        }
        Navigation.openPost(root.id);
    }

    ListView.onReused: {
        postContent.visible = Qt.binding(() => {
            return root.spoilerText.length === 0 || AccountManager.selectedAccount.preferences.extendSpoiler;
        });
        filtered = Qt.binding(() => {
            return root.filters.length > 0;
        });
    }

    background: Rectangle {
        color: root.selected ? Kirigami.Theme.alternateBackgroundColor : Kirigami.Theme.backgroundColor

        Kirigami.Separator {
            width: flexColumn.innerWidth
            visible: root.showSeparator && !root.selected
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }
        }
    }

    TapHandler {
        id: tapHandler

        onTapped: eventPoint => {
            // Get the inner ColumnLayout for the FlexColumn
            const innerChild = flexColumn.children[0];
            // Check if we're inside the inner column, not on the outside:
            if (innerChild.contains(innerChild.mapFromGlobal(eventPoint.globalPosition.x, eventPoint.globalPosition.y))) {
                root.openPost();
            }
        }
    }

    contentItem: PostLayout {
        id: flexColumn

        isThreadReply: false
        isReply: false
        threadMargin: 0
        isLastThreadReply: false

        RowLayout {
            id: extraItemsLayout

            spacing: Kirigami.Units.smallSpacing
        }

        // TODO: create notification-specific delegate for this! and then use the new children default property
        RowLayout {
            visible: root.notificationActorIdentity !== undefined && root.relativeTime !== ""
            spacing: 0

            // Interaction labels for notifications
            Loader {
                active: root.notificationActorIdentity !== undefined && !root.isGroup
                visible: active

                sourceComponent: Notifications.UserInteractionLabel {
                    type: root.type
                    notificationActorIdentity: root.notificationActorIdentity
                }
            }

            // Interaction labels for grouped notifications
            Loader {
                active: root.notificationActorIdentity !== undefined && root.isGroup
                visible: active

                sourceComponent: Notifications.GroupInteractionLabel {
                    type: root.type
                    notificationActorIdentity: root.notificationActorIdentity
                    numInGroup: root.numInGroup
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

        FilterNotice {
            filters: root.filters
            filtered: root.filtered

            onToggleFilter: root.filtered = false

            Layout.fillWidth: true
        }

        ColumnLayout {
            visible: !root.filtered && root.hasContent
            spacing: Kirigami.Units.largeSpacing

            Layout.fillWidth: true

            Loader {
                visible: root.spoilerText.length !== 0
                active: visible

                Layout.fillWidth: true

                sourceComponent: ContentNotice {
                    postContent: postContent
                    spoilerText: root.spoilerText

                    onToggleNotice: postContent.visible = !postContent.visible
                }
            }

            PostContent {
                id: postContent

                content: root.content
                expandedPost: false
                secondary: true
                visible: root.spoilerText.length === 0 || AccountManager.selectedAccount.preferences.extendSpoiler
                shouldOpenInternalLinks: true
            }
        }

        Loader {
            sourceComponent: PostPoll {
                index: root.index
                poll: root.poll
            }

            active: root.poll !== null && root.poll !== undefined
            visible: active && postContent.visible

            Layout.fillWidth: true
        }

        Loader {
            sourceComponent: AttachmentGrid {
                expandedPost: false
                attachments: root.attachments
                identity: root.authorIdentity
                sensitive: root.sensitive
                secondary: false
                inViewPort: root.inViewPort
                viewportWidth: flexColumn.innerWidth
                forceCrop: true
            }

            active: root.attachments.length > 0 && !filtered
            visible: active

            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        PostTags {
            standaloneTags: root.standaloneTags
            visible: !root.filtered && postContent.visible && root.standaloneTags.length !== 0

            Layout.fillWidth: true
        }
    }
}
