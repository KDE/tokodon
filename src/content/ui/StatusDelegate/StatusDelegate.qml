// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.tokodon.private

import "../Notifications" as Notifications

QQC2.ItemDelegate {
    id: root

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

    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    leftPadding: width > Kirigami.Units.gridUnit * 25 ? Kirigami.Units.largeSpacing * 2 : Kirigami.Units.largeSpacing
    rightPadding: width > Kirigami.Units.gridUnit * 25 ? Kirigami.Units.largeSpacing * 2 : Kirigami.Units.largeSpacing

    highlighted: false
    hoverEnabled: false
    width: ListView.view.width

    Kirigami.Theme.colorSet: root.selected ? Kirigami.Theme.Window : Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    Accessible.description: root.spoilerText.length === 0 ? i18n("Normal Status") : i18n("Spoiler Status")

    onClicked: {
        if (postContent.hoveredLink) {
            return;
        }
        Navigation.openThread(root.id);
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
        color: Kirigami.Theme.backgroundColor

        Kirigami.Separator {
            visible: root.showSeparator && !root.selected
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
        }
    }

    contentItem: Kirigami.FlexColumn {
        spacing: Kirigami.Units.largeSpacing

        maximumWidth: Kirigami.Units.gridUnit * 40

        RowLayout {
            visible: filtered
            Layout.fillWidth: true
            QQC2.Label {
                font: Config.defaultFont
                Layout.alignment: Qt.AlignHCenter
                text: i18n("Filtered: %1", root.filters.join(', '))
            }
            Kirigami.LinkButton {
                Layout.alignment: Qt.AlignHCenter
                text: i18n("Show anyway")
                onClicked: filtered = false
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: visible ? Kirigami.Units.smallSpacing : 0
            visible: root.pinned && !root.filtered
            Kirigami.Icon {
                source: "pin"
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }
            QQC2.Label {
                font: Config.defaultFont
                text: i18n("Pinned entry")
                color: Kirigami.Theme.disabledTextColor
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }

        // Normal interaction labels on the timeline
        Loader {
            sourceComponent: UserInteractionLabel {
                isBoosted: root.isBoosted
                isReply: root.isReply
                type: root.type
                boostAuthorIdentity: root.boostAuthorIdentity
                replyAuthorIdentity: root.replyAuthorIdentity
            }
            active: !root.notificationActorIdentity && (root.isBoosted || root.isReply)
            visible: active
        }

        // Interaction labels for notifications
        Loader {
            sourceComponent: Notifications.UserInteractionLabel
            {
                type: root.type
                notificationActorIdentity: root.notificationActorIdentity
            }
            active: root.notificationActorIdentity !== undefined && !root.isGroup
            visible: active
        }

        // Interaction labels for grouped notifications
        Loader {
            sourceComponent: Notifications.GroupInteractionLabel
            {
                type: root.type
                notificationActorIdentity: root.notificationActorIdentity
                numInGroup: root.numInGroup
            }
            active: root.notificationActorIdentity !== undefined && root.isGroup
            visible: active
        }

        InlineIdentityInfo {
            identity: root.authorIdentity
            secondary: root.secondary
            visible: !filtered

            InteractionButton {
                Layout.alignment: Qt.AlignVCenter
                iconName: switch(root.visibility) {
                    case Post.Public:
                        return "kstars_xplanet";
                    case Post.Unlisted:
                        return "unlock";
                    case Post.Private:
                        return "lock";
                    case Post.Direct:
                        return "mail-message";
                    default:
                        return "kstars_xplanet";
                }
                tooltip: switch(root.visibility) {
                    case Post.Public:
                        return i18n("Public");
                    case Post.Unlisted:
                        return i18n("Unlisted");
                    case Post.Private:
                        return i18n("Private");
                    case Post.Direct:
                        return i18n("Direct Message");
                    default:
                        return i18n("Public");
                }
                interactable: false
            }

            Kirigami.Heading {
                id: heading
                font.pixelSize: Config.defaultFont.pixelSize + 1
                font.pointSize: -1
                text: root.relativeTime
                color: root.secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
                Layout.alignment: Qt.AlignBaseline
                elide: Text.ElideRight
            }

            InteractionButton {
                Layout.alignment: Qt.AlignVCenter
                Layout.preferredWidth: implicitHeight
                iconName: 'overflow-menu'
                tooltip: i18nc("Show more options", "More")
                onClicked: {
                    postMenu.active = true;
                    postMenu.item.open()
                }
                Loader {
                    id: postMenu

                    active: false
                    visible: active

                    sourceComponent: OverflowMenu {
                        index: root.index
                        postId: root.id
                        url: root.url
                        bookmarked: root.bookmarked
                        isSelf: root.isSelf
                        expandedPost: root.expandedPost
                        pinned: root.pinned

                        onClosed: postMenu.active = false
                    }
                }
            }
        }

        ColumnLayout {
            visible: !filtered && root.content.length !== 0
            spacing: Kirigami.Units.largeSpacing

            Layout.fillWidth: true

            QQC2.Control {
                Layout.fillWidth: true
                visible: root.spoilerText.length !== 0
                contentItem: RowLayout {
                    Kirigami.Icon {
                        Layout.alignment: Qt.AlignVCenter
                        source: "data-warning"
                    }

                    QQC2.Label {
                        id: spoilerTextLabel
                        Layout.fillWidth: true
                        text: i18n("<b>Content Warning</b><br /> %1", root.spoilerText)
                        wrapMode: Text.Wrap
                        font: Config.defaultFont

                    }

                    QQC2.Button {
                        text: postContent.visible ? i18n("Show Less") : i18n("Show More")
                        onClicked: postContent.visible = !postContent.visible
                    }
                }

                background: Rectangle {
                    radius: Kirigami.Units.largeSpacing
                    color: Kirigami.Theme.neutralBackgroundColor
                }
            }

            PostContent {
                id: postContent

                content: root.content
                expandedPost: root.expandedPost
                secondary: root.secondary
                visible: root.spoilerText.length === 0 || AccountManager.selectedAccount.preferences.extendSpoiler
                shouldOpenInternalLinks: true

                onClicked: root.clicked()
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
            }
            active: !root.secondary && root.attachments.length > 0 && !filtered
            visible: active

            Layout.fillWidth: true
        }

        RowLayout {
            id: buttonLayout

            readonly property bool shouldExpand: Kirigami.Settings.isMobile || Kirigami.Settings.tabletMode
            readonly property real buttonPadding: shouldExpand ? Kirigami.Units.mediumSpacing : Kirigami.Units.smallSpacing

            visible: showInteractionButton && !filtered
            Layout.fillWidth: true
            spacing: shouldExpand ? 0 : Kirigami.Units.gridUnit * 2

            InteractionButton {
                topPadding: buttonLayout.buttonPadding
                bottomPadding: buttonLayout.buttonPadding
                rightPadding: buttonLayout.buttonPadding
                leftPadding: buttonLayout.buttonPadding

                interacted: root.repliesCount > 0
                iconName: "tokodon-post-reply"
                interactedIconName: "tokodon-post-reply-filled"

                tooltip: i18nc("Reply to a post", "Reply")
                text: Config.showPostStats && !root.selected ? root.repliesCount : ''

                onClicked: Navigation.replyTo(root.id, root.mentions, root.visibility, root.authorIdentity, root.post)
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
                interactionColor: "green"

                enabled: root.visibility !== Post.Direct

                iconName: 'tokodon-post-boost'
                interactedIconName: 'tokodon-post-boosted'

                text: Config.showPostStats && !root.selected ? root.reblogsCount : ''
                tooltip: i18nc("Share a post", "Boost")

                onClicked: timelineModel.actionRepeat(timelineModel.index(root.index, 0))
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
                interactionColor: "orange"

                iconName: 'tokodon-post-favorite'
                interactedIconName: 'tokodon-post-favorited'

                text: Config.showPostStats && !root.selected ? root.favouritesCount : ''
                tooltip: i18nc("Favorite a post", "Favorite")

                onClicked: timelineModel.actionFavorite(timelineModel.index(root.index, 0))
                Accessible.description: root.favourited ? i18n("Favourited") : i18n("Favourite")
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
                interactionColor: "red"

                iconName: 'bookmarks'
                interactedIconName: 'tokodon-post-bookmarked'

                tooltip: root.bookmarked ? i18n("Remove bookmark") : i18nc("Bookmark a post", "Bookmark")

                onClicked: timelineModel.actionBookmark(timelineModel.index(root.index, 0))
                Accessible.description: root.bookmarked ? i18n("Bookmarked") : i18n("Bookmark")
            }
        }

        RowLayout {
            visible: root.expandedPost && root.selected

            QQC2.Label {
                text: root.absoluteTime
                elide: Text.ElideRight
                color: Kirigami.Theme.disabledTextColor
            }

            QQC2.Label {
                visible: root.application && root.application.name
                text: root.application && root.application.name ? i18n("via %1", root.application.name) : ''
                elide: Text.ElideRight
                Layout.fillWidth: true
                color: Kirigami.Theme.disabledTextColor

                HoverHandler {
                    cursorShape: hasWebsite ? Qt.PointingHandCursor: Qt.ArrowCursor
                    onHoveredChanged: if (hovered) {
                        applicationWindow().hoverLinkIndicator.text = root.application.website;
                    } else {
                        applicationWindow().hoverLinkIndicator.text = "";
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: hasWebsite ? Qt.PointingHandCursor: Qt.ArrowCursor
                    onClicked: Qt.openUrlExternally(root.application.website)
                }

            }
        }

        Item {
            height: Kirigami.Units.mediumSpacing
            visible: root.selected && (root.repliesCount > 0 || root.reblogsCount > 0 || root.favouritesCount > 0)
        }

        RowLayout {
            visible: root.expandedPost && root.selected
            InteractionButton {
                visible: root.repliesCount > 0
                iconName: "tokodon-post-reply-filled"
                tooltip: i18np("%1 Reply", "%1 Replies", root.repliesCount)
                text: i18np("%1 Reply", "%1 Replies", root.repliesCount)
                enabled: false
                textColor: Kirigami.Theme.disabledTextColor
                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                }
            }

            InteractionButton {
                visible: root.favouritesCount > 0
                iconName: "tokodon-post-favorited"
                tooltip: i18np("%1 Favorite", "%1 Favorites", root.favouritesCount)
                text: i18np("%1 Favorite", "%1 Favorites", root.favouritesCount)
                enabled: true
                textColor: Kirigami.Theme.disabledTextColor
                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                }
                onClicked: {
                    pageStack.push(socialGraphComponent, {
                        name: "favourited_by",
                        statusId: root.id,
                        count: root.favouritesCount
                    });
                }
            }

            InteractionButton {
                visible: root.reblogsCount > 0
                iconName: "tokodon-post-boosted"
                tooltip: i18np("%1 Boost", "%1 Boosts", root.reblogsCount)
                text: i18np("%1 Boost", "%1 Boosts", root.reblogsCount)
                enabled: true
                textColor: Kirigami.Theme.disabledTextColor
                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                }
                onClicked: {
                    pageStack.push(socialGraphComponent, {
                        name: "reblogged_by",
                        statusId: root.id,
                        count: root.reblogsCount
                    });
                }
            }
        }
    }
}
