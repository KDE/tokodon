// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import org.kde.kirigamiaddons.components 1.0 as KirigamiComponents
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

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
            visible: (root.type === Notification.Favorite || root.type === Notification.Update || root.type === Notification.Poll) && !root.filtered
            Kirigami.Icon {
                source: if (root.type === Notification.Favorite) {
                    return "favorite"
                } else if (root.type === Notification.Update) {
                    return "cell_edit"
                } else if (root.type === Notification.Poll) {
                    return "office-chart-bar"
                }
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                color: Kirigami.Theme.disabledTextColor
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }

            QQC2.AbstractButton {
                contentItem: RowLayout {
                    KirigamiComponents.AvatarButton {
                        implicitHeight: Math.round(Kirigami.Units.gridUnit * 1.5)
                        implicitWidth: implicitHeight
                        Layout.alignment: Qt.AlignTop
                        Layout.bottomMargin: -Kirigami.Units.gridUnit
                        source: root.notificationActorIdentity && root.notificationActorIdentity.avatarUrl ? root.notificationActorIdentity.avatarUrl :  ''
                        cache: true
                        QQC2.ToolTip.text: i18n("View profile")
                        onClicked: Navigation.openAccount(root.notificationActorIdentity.id)
                        name: root.notificationActorIdentity && root.notificationActorIdentity.displayName ? root.notificationActorIdentity.displayName :  ''
                        visible: [Notification.Favorite, root.type === Notification.Update].includes(root.type)
                    }
                    QQC2.Label {
                        font: Config.defaultFont
                        text: if (root.type === Notification.Favorite) {
                            return i18n("%1 favorited your post", root.notificationActorIdentity.displayNameHtml)
                        } else if (root.type === Notification.Update) {
                            return i18n("%1 edited a post", root.notificationActorIdentity.displayNameHtml)
                        } else if (root.type === Notification.Poll) {
                            if (root.isSelf) {
                                return i18n("Your poll has ended")
                            } else {
                                return i18n("A poll you voted in has ended")
                            }
                        } else {
                            return ''
                        }
                        textFormat: Text.RichText
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                    }
                }
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

        RowLayout {
            id: interactLayout

            readonly property bool isBoost: root.isBoosted || root.type === Notification.Repeat
            readonly property bool isReply: root.isReply || root.type === Notification.Reply

            visible: {
                if (filtered) {
                    return false
                }

                return (interactLayout.isBoost && root.boostAuthorIdentity) || (interactLayout.isReply && root.replyAuthorIdentity)
            }

            Layout.fillWidth: true
            Layout.bottomMargin: visible ? Kirigami.Units.smallSpacing : 0
            Kirigami.Icon {
                source: {
                    if (interactLayout.isBoost) {
                        return "post-boost"
                    } else if (interactLayout.isReply) {
                        return "post-reply"
                    }

                    return ''
                }
                isMask: true
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                color: root.type === Notification.Repeat ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }

            QQC2.AbstractButton {
                contentItem: RowLayout {
                    KirigamiComponents.AvatarButton {
                        implicitHeight: Math.round(Kirigami.Units.gridUnit * 1.5)
                        implicitWidth: implicitHeight
                        Layout.alignment: Qt.AlignTop
                        Layout.bottomMargin: -Kirigami.Units.gridUnit
                        source: {
                            if (interactLayout.isBoost && root.boostAuthorIdentity) {
                                return root.boostAuthorIdentity.avatarUrl ? root.boostAuthorIdentity.avatarUrl : ''
                            } else if (interactLayout.isReply && root.replyAuthorIdentity) {
                                return root.replyAuthorIdentity.avatarUrl ? root.replyAuthorIdentity.avatarUrl : ''
                            }

                            return ''
                        }
                        cache: true
                        QQC2.ToolTip.text: i18n("View profile")
                        onClicked: Navigation.openAccount(root.boostAuthorIdentity.id)
                        name: root.boostAuthorIdentity && root.boostAuthorIdentity.displayName ? root.boostAuthorIdentity.displayName : ''
                    }
                    QQC2.Label {
                        text: {
                            if (interactLayout.isBoost) {
                                return root.boostAuthorIdentity ? i18n("%1 boosted", root.boostAuthorIdentity.displayNameHtml) : (root.type === Notification.Repeat ? i18n("%1 boosted your post", root.notificationActorIdentity.displayNameHtml) : '')
                            } else if (interactLayout.isReply) {
                                return root.replyAuthorIdentity ? i18n("In reply to %1", root.replyAuthorIdentity.displayNameHtml) : (root.type === Notification.Reply ? i18n("%1 replied to your post", root.notificationActorIdentity.displayNameHtml) : '')
                            }

                            return ''
                        }
                        color: root.type === Notification.Repeat ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                        font: Config.defaultFont
                        Layout.alignment: Qt.AlignVCenter
                        Layout.fillWidth: true
                    }
                }
            }
        }

        InlineIdentityInfo {
            identity: root.authorIdentity
            secondary: root.secondary
            visible: !filtered

            InteractionButton {
                Layout.alignment: Qt.AlignTop
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
                verticalAlignment: Text.AlignTop
                topPadding: Kirigami.Units.smallSpacing
                Layout.alignment: Qt.AlignTop
                elide: Text.ElideRight
            }

            InteractionButton {
                Layout.alignment: Qt.AlignTop
                iconName: 'overflow-menu'
                tooltip: i18nc("Show more options", "More")
                onClicked: {
                    postMenu.active = true;
                    postMenu.item.open()
                }
                Loader {
                    id: postMenu

                    active: false

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
            spacing: 0

            Layout.fillWidth: true

            RowLayout {
                Layout.preferredHeight: spoilerTextLabel.contentHeight + Kirigami.Units.gridUnit * 2
                visible: root.spoilerText.length !== 0
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

        Loader {
            sourceComponent: LinkPreview {
                card: root.card
            }

            active: Config.showLinkPreview && card && !root.secondary && root.post.attachments.length === 0 && !root.filtered

            visible: active && postContent.visible
            Layout.fillWidth: true
        }

        Loader {
            sourceComponent: StatusPoll {
                index: root.index
                poll: root.poll
            }

            visible: active
            active: root.poll !== undefined && !root.filtered
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
                iconName: "post-reply"
                interactedIconName: "post-reply-filled"

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

                iconName: 'post-boost'
                interactedIconName: 'post-boosted'

                text: Config.showPostStats && !root.selected ? root.reblogsCount : ''
                tooltip: i18nc("Share a post", "Boost")

                onClicked: timelineModel.actionRepeat(timelineModel.index(root.index, 0))
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

                iconName: 'post-favorite'
                interactedIconName: 'post-favorited'

                text: Config.showPostStats && !root.selected ? root.favouritesCount : ''
                tooltip: i18nc("Favorite a post", "Favorite")

                onClicked: timelineModel.actionFavorite(timelineModel.index(root.index, 0))
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
                interactedIconName: 'post-bookmarked'

                tooltip: root.bookmarked ? i18n("Remove bookmark") : i18nc("Bookmark a post", "Bookmark")

                onClicked: timelineModel.actionBookmark(timelineModel.index(root.index, 0))
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
                iconName: "post-reply-filled"
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
                iconName: "post-favorited"
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
                iconName: "post-boosted"
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

        QQC2.ProgressBar {
            visible: root.loading && !root.showSeparator
            indeterminate: true
            padding: Kirigami.Units.largeSpacing * 2
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
        }
    }
}
