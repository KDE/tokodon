// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon


import "../Notifications" as Notifications

/**
 * @brief The main status delegate component. Many of its contents are in reusable subcomponents though.
 */
QQC2.ItemDelegate {
    id: root

    // The model roles
    required property int index

    required property string id
    required property string originalId
    required property string url
    required property var authorIdentity

    required property bool isBoosted
    required property var boostAuthorIdentity

    required property bool isReply
    required property var replyAuthorIdentity

    required property var notificationActorIdentity

    // Interaction count
    required property int reblogsCount
    required property int repliesCount
    required property int favouritesCount
    required property int quotesCount

    // User self interaction
    required property bool favourited
    required property bool reblogged
    required property bool muted
    required property bool bookmarked
    required property bool pinned

    required property string content
    required property string spoilerText
    required property string relativeTime
    required property string absoluteTime
    required property string publishedAt
    required property var attachments
    required property var poll
    required property var card
    required property var application
    required property bool selected
    required property var filters
    required property bool sensitive
    required property int type
    required property var mentions
    required property int visibility
    required property bool wasEdited
    required property string editedAt
    required property bool isThreadReply
    required property bool isLastThreadReply

    required property var post

    required property bool isGroup
    required property bool isInGroup
    required property int numInGroup

    property bool filtered: root.filters.length > 0
    property var timelineModel
    property bool secondary: false
    property bool showSeparator: true
    property bool showInteractionButton: true
    property bool expandedPost: false
    property bool loading: false
    property bool inViewPort: true
    property bool hasWebsite: root.application && root.application.website !== undefined && root.application.website.toString().trim().length > 0

    readonly property bool isSelf: AccountManager.selectedAccount.identity === root.authorIdentity
    readonly property real threadMargin: Kirigami.Units.largeSpacing * 4

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
            readonly property bool isReply: root.expandedPost && (root.isThreadReply && !root.isLastThreadReply)

            width: isReply ? flexColumn.innerWidth - root.threadMargin : flexColumn.innerWidth
            visible: root.showSeparator && !root.selected
            anchors {
                horizontalCenter: parent.horizontalCenter
                horizontalCenterOffset: isReply ? (root.threadMargin / 2) : 0
                bottom: parent.bottom
            }
        }
    }

    onReleased: {
        // Get the inner ColumnLayout for the FlexColumn
        const innerChild = flexColumn.children[0];
        // Check if we're inside the inner column, not on the outside:
        if (innerChild.contains(innerChild.mapFromItem(root, root.pressX, root.pressY))) {
            root.openPost();
        }
    }

    contentItem: PostLayout {
        id: flexColumn

        isThreadReply: root.isThreadReply
        isReply: root.isReply
        threadMargin: root.threadMargin
        isLastThreadReply: root.isLastThreadReply

        RowLayout {
            spacing: Kirigami.Units.smallSpacing
            visible: root.pinned && !root.notificationActorIdentity && !root.filtered

            Layout.fillWidth: true
            Layout.bottomMargin: visible ? Kirigami.Units.smallSpacing : 0

            Kirigami.Icon {
                source: "pin"
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }

            QQC2.Label {
                font: Config.defaultFont
                text: i18nc("@label", "Pinned Post")
                color: Kirigami.Theme.disabledTextColor
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }

        // Normal interaction labels on the timeline
        Loader {
            sourceComponent: PostInteractionLabel {
                isBoosted: root.isBoosted
                isReply: root.isReply
                type: root.type
                boostAuthorIdentity: root.boostAuthorIdentity
                replyAuthorIdentity: root.replyAuthorIdentity
            }
            active: !root.expandedPost && !root.notificationActorIdentity && (root.isBoosted || root.isReply)
            visible: active
        }

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

        PostInfoBar {
            id: infoBar

            identity: root.authorIdentity
            secondary: root.secondary
            relativeTime: root.relativeTime
            selected: root.selected
            wasEdited: root.wasEdited
            editedAt: root.editedAt
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

        readonly property Component postMenu: PostMenu {
            index: root.index
            postId: root.id
            url: root.url
            isSelf: root.isSelf
            expandedPost: root.expandedPost
            pinned: root.pinned
            authorIdentity: root.authorIdentity
            isPrivate: root.visibility === Post.Direct || root.visibility === Post.Private
            muted: root.muted

            onDeletePost: {
                deleteDialog.active = true;
                deleteDialog.item.open()
            }
            onRedraftPost: {
                redraftDialog.active = true;
                redraftDialog.item.open()
            }
        }

        FilterNotice {
            filters: root.filters
            filtered: root.filtered

            onToggleFilter: root.filtered = false

            Layout.fillWidth: true
        }

        ColumnLayout {
            visible: !root.filtered && root.post.hasContent
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
                expandedPost: root.expandedPost
                secondary: root.secondary
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
            // note: using from post object instead of timeline model because it's loaded async
            active: root.post.quotedPost !== null && postContent.visible && root.post.attachments.length === 0
            visible: active

            Layout.fillWidth: true

            sourceComponent: QuotePost {
                post: root.post.quotedPost
            }
        }

        Loader {
            sourceComponent: AttachmentGrid {
                expandedPost: root.expandedPost
                attachments: root.post.attachments
                identity: root.authorIdentity
                sensitive: root.sensitive
                secondary: root.secondary
                inViewPort: root.inViewPort
                viewportWidth: flexColumn.innerWidth
            }

            active: !root.secondary && root.attachments.length > 0 && !filtered
            visible: active

            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        Loader {
            active: Config.showLinkPreview && card && !root.secondary && root.post.attachments.length === 0 && !root.filtered && !root.post.quotedPost
            visible: active && postContent.visible

            Layout.fillWidth: true

            sourceComponent: LinkPreview {
                card: root.card
                selected: root.selected
            }
        }

        PostTags {
            standaloneTags: root.post.standaloneTags
            visible: !root.filtered && postContent.visible && root.post.standaloneTags.length !== 0

            Layout.fillWidth: true
        }

        RowLayout {
            id: buttonLayout

            readonly property bool shouldExpand: !(root.QQC2.ApplicationWindow.window as Main)?.wideMode ?? false
            readonly property real buttonPadding: shouldExpand ? Kirigami.Units.mediumSpacing : Kirigami.Units.smallSpacing

            visible: root.showInteractionButton
            Layout.fillWidth: true
            spacing: shouldExpand ? 0 : Kirigami.Units.gridUnit * 2

            InteractionButton {
                topPadding: buttonLayout.buttonPadding
                bottomPadding: buttonLayout.buttonPadding
                rightPadding: buttonLayout.buttonPadding
                leftPadding: buttonLayout.buttonPadding

                interacted: root.repliesCount > 0
                iconName: "view-conversation-balloon-symbolic"
                interactedIconName: "view-conversation-balloon-symbolic"

                tooltip: i18nc("Reply to a post", "Reply")
                text: root.repliesCount

                onClicked: Navigation.replyTo(root.post)
            }

            Item {
                Layout.fillWidth: buttonLayout.shouldExpand
            }

            InteractionButton {
                topPadding: buttonLayout.buttonPadding
                bottomPadding: buttonLayout.buttonPadding
                leftPadding: buttonLayout.buttonPadding
                rightPadding: buttonLayout.buttonPadding

                interacted: root.reblogged
                interactionColor: "#63c605"

                enabled: root.visibility !== Post.Direct && (root.visibility !== Post.Private || root.isSelf)

                iconName: 'boost'
                interactedIconName: 'boost-boosted'

                text: Config.showPostStats && !root.selected ? root.reblogsCount : ''
                tooltip: {
                    if (root.visibility === Post.Direct) {
                        return i18n("Cannot boost direct messages");
                    } else if (root.visibility === Post.Private && !root.isSelf) {
                        return i18n("Cannot boost private posts");
                    } else {
                        return i18nc("Share a post", "Boost");
                    }
                }

                onClicked: {
                    if (Config.askBeforeBoosting && !root.reblogged) {
                        const dialog = Qt.createComponent("org.kde.tokodon", "BoostConfirmationDialog").createObject(QQC2.Overlay.overlay, {
                            sourceIdentity: AccountManager.selectedAccount.identity,
                            targetIdentity: root.authorIdentity
                        });
                        dialog.accepted.connect(function() {
                            root.timelineModel.actionRepeat(timelineModel.index(root.index, 0))
                        });
                        dialog.visible = true;
                    } else {
                        root.timelineModel.actionRepeat(timelineModel.index(root.index, 0))
                    }
                }
                Accessible.description: root.reblogged ? i18n("Boosted") : i18n("Boost")
            }

            Item {
                Layout.fillWidth: buttonLayout.shouldExpand
            }

            InteractionButton {
                topPadding: buttonLayout.buttonPadding
                bottomPadding: buttonLayout.buttonPadding
                leftPadding: buttonLayout.buttonPadding
                rightPadding: buttonLayout.buttonPadding

                interacted: root.favourited
                interactionColor: "#fa8865"

                iconName: 'favorite'
                interactedIconName: 'favorite-favorited'

                text: Config.showPostStats && !root.selected ? root.favouritesCount : ''
                tooltip: i18nc("Favorite a post", "Favorite")

                onClicked: root.timelineModel.actionFavorite(timelineModel.index(root.index, 0))
                Accessible.description: root.favourited ? i18n("Favorited") : i18n("Favorite")
            }

            Item {
                Layout.fillWidth: buttonLayout.shouldExpand
            }

            InteractionButton {
                topPadding: buttonLayout.buttonPadding
                bottomPadding: buttonLayout.buttonPadding
                leftPadding: buttonLayout.buttonPadding
                rightPadding: buttonLayout.buttonPadding

                interacted: root.bookmarked
                interactionColor: "#1b89f3"

                iconName: 'bookmarks'
                interactedIconName: 'bookmarks-bookmarked'

                tooltip: root.bookmarked ? i18n("Remove bookmark") : i18nc("Bookmark a post", "Bookmark")

                onClicked: root.timelineModel.actionBookmark(timelineModel.index(root.index, 0))
                Accessible.description: root.bookmarked ? i18n("Bookmarked") : i18n("Bookmark")
            }
        }

        Loader {
            active: root.selected
            visible: root.selected

            Layout.fillWidth: true

            sourceComponent: InformationBar {
                postId: root.id
                selected: root.selected
                visibility: root.visibility
                wasEdited: root.wasEdited
                editedAt: root.editedAt
                favouritesCount: root.favouritesCount
                reblogsCount: root.reblogsCount
                quotesCount: root.quotesCount
                application: root.application
                absoluteTime: root.absoluteTime
            }
        }

        Loader {
            active: root.selected && root.visibility === Post.Private
            visible: active

            Layout.fillWidth: true
            Layout.preferredWidth: visible ? flexColumn.innerWidth : -1 // prevent it from messing with the width of the post overall

            sourceComponent: Kirigami.InlineMessage {
                id: privacyWarning

                text: i18n("This post has been marked private. Some posts may be missing because its replies are marked as private by default.")
                visible: true
            }
        }
    }
}
