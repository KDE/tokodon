// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
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

    Accessible.description: root.spoilerText.length == 0 ? i18n("Normal Status") : i18n("Spoiler Status");

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
    }

    contentItem: Kirigami.FlexColumn {
        spacing: 0

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
                    return "folder-chart"
                }
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                color: Kirigami.Theme.disabledTextColor
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }

            QQC2.AbstractButton {
                contentItem: RowLayout {
                    Kirigami.Avatar {
                        implicitHeight: Math.round(Kirigami.Units.gridUnit * 1.5)
                        implicitWidth: implicitHeight
                        Layout.alignment: Qt.AlignTop
                        Layout.bottomMargin: -Kirigami.Units.gridUnit
                        source: root.notificationActorIdentity && root.notificationActorIdentity.avatarUrl ? root.notificationActorIdentity.avatarUrl :  ''
                        cache: true
                        actions.main: Kirigami.Action {
                            tooltip: i18n("View profile")
                            onTriggered: Navigation.openAccount(root.notificationActorIdentity.id)
                        }
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

                return interactLayout.isBoost || interactLayout.isReply
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
                    Kirigami.Avatar {
                        implicitHeight: Math.round(Kirigami.Units.gridUnit * 1.5)
                        implicitWidth: implicitHeight
                        Layout.alignment: Qt.AlignTop
                        Layout.bottomMargin: -Kirigami.Units.gridUnit
                        source: {
                            if (interactLayout.isBoost) {
                                return root.boostAuthorIdentity.avatarUrl ? root.boostAuthorIdentity.avatarUrl : ''
                            } else if (interactLayout.isReply) {
                                return root.replyAuthorIdentity.avatarUrl ? root.replyAuthorIdentity.avatarUrl : ''
                            }

                            return ''
                        }
                        cache: true
                        actions.main: Kirigami.Action
                            {
                            tooltip: i18n("View profile")
                            onTriggered: Navigation.openAccount(root.boostAuthorIdentity.id)
                        }
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

            Kirigami.Heading {
                id: heading
                font.pixelSize: Config.defaultFont.pixelSize + 1
                font.pointSize: -1
                text: root.relativeTime
                color: root.secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
                verticalAlignment: Text.AlignTop
                Layout.alignment: Qt.AlignTop
                elide: Text.ElideRight
            }
        }

        ColumnLayout {
            visible: !filtered

            RowLayout {
                visible: root.spoilerText.length !== 0
                QQC2.Label {
                    Layout.fillWidth: true
                    text: root.spoilerText
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

        AttachmentGrid {
            expandedPost: root.expandedPost
            attachments: root.post.attachments
            identity: root.authorIdentity
            sensitive: root.sensitive
            secondary: root.secondary
            inViewPort: root.inViewPort

            visible: postContent.visible && !root.secondary && root.attachments.length > 0 && !filtered
        }

        LinkPreview {
            attachments: root.attachments
            card: root.card
            secondary: root.secondary
            contentVisible: postContent.visible
        }

        Poll {
            index: root.index
            poll: root.poll
        }

        RowLayout {
            id: buttonLayout

            visible: showInteractionButton && !filtered
            Layout.fillWidth: true
            spacing: 0

            readonly property bool shouldExpand: applicationWindow().width < Kirigami.Units.gridUnit * 50

            InteractionButton {
                Layout.fillWidth: buttonLayout.shouldExpand

                iconName: "post-reply"

                tooltip: i18nc("Reply to a post", "Reply")
                text: root.repliesCount < 2 ? root.repliesCount : (Config.showPostStats || root.selected ? root.repliesCount : i18nc("More than one reply", "1+"))

                onClicked: Navigation.replyTo(root.id, root.mentions, root.visibility, root.authorIdentity, root.post)
            }
            InteractionButton {
                Layout.fillWidth: buttonLayout.shouldExpand

                interacted: root.reblogged
                interactionColor: "green"

                iconName: 'post-boost'
                interactedIconName: 'post-boosted'

                text: (Config.showPostStats || root.selected) ? root.reblogsCount : ''
                tooltip: i18nc("Share a post", "Boost")

                onClicked: timelineModel.actionRepeat(timelineModel.index(root.index, 0))
                Accessible.description: root.reblogged ? i18n("Boosted") : i18n("Boost");
            }
            InteractionButton {
                Layout.fillWidth: buttonLayout.shouldExpand

                interacted: root.favourited
                interactionColor: "orange"

                iconName: 'post-favorite'
                interactedIconName: 'post-favorited'

                text: (Config.showPostStats || root.selected) ? root.favouritesCount : ''
                tooltip: i18nc("Like a post", "Like")

                onClicked: timelineModel.actionFavorite(timelineModel.index(root.index, 0))
                Accessible.description: root.favourited ? i18n("Favourited") : i18n("Favourite");
            }
            InteractionButton {
                Layout.fillWidth: buttonLayout.shouldExpand

                interacted: root.bookmarked
                interactionColor: "red"

                iconName: 'bookmarks'
                interactedIconName: 'post-bookmarked'

                tooltip: root.bookmarked ? i18n("Remove bookmark") : i18nc("Bookmark a post", "Bookmark")

                onClicked: timelineModel.actionBookmark(timelineModel.index(root.index, 0))
                Accessible.description: root.bookmarked ? i18n("Bookmarked") : i18n("Bookmark");
            }
            InteractionButton {
                rightPadding: 0

                iconName: 'overflow-menu'

                tooltip: i18nc("Show more options", "More")

                onClicked: postMenu.open()

                OverflowMenu {
                    id: postMenu
                    index: root.index
                    postId: root.id
                    url: root.url
                    bookmarked: root.bookmarked
                    isSelf: root.isSelf
                    expandedPost: root.expandedPost
                }
            }
        }

        RowLayout {
            visible: root.expandedPost && root.selected
            QQC2.ToolButton {
                icon.name: 
                    switch(root.visibility) {
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
                text: i18n("Visibility")
                display: QQC2.AbstractButton.IconOnly
                QQC2.ToolTip.text: switch(root.visibility) {
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
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                QQC2.ToolTip.visible: hovered
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }

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

        Kirigami.Separator {
            visible: root.showSeparator && !root.selected
            Layout.fillWidth: true
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
