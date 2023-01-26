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
    required property string url
    required property var authorIdentity

    required property bool isBoosted
    required property var boostAuthorIdentity

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
    required property var attachments
    required property var poll
    required property var card
    required property bool selected
    required property var filters
    required property bool sensitive
    required property int type
    required property var mentions
    required property int visibility

    property bool filtered: root.filters.length > 0
    property var timelineModel
    property bool secondary: false
    property bool showSeparator: true
    property bool showInteractionButton: true
    property bool expandedPost: false

    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    leftPadding: Kirigami.Units.largeSpacing * 2
    rightPadding: Kirigami.Units.largeSpacing * 2

    highlighted: false
    hoverEnabled: false
    width: ListView.view.width

    Kirigami.Theme.colorSet: root.selected ? Kirigami.Theme.Window : Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    onClicked: {
        if (tootContent.hoveredLink) {
            return;
        }
        Navigation.openThread(root.id);
    }

    ListView.onReused: {
        tootContent.visible = Qt.binding(() => {
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
                        text: if (root.type === Notification.Favorite) {
                            return i18n("%1 favorited your post", root.notificationActorIdentity.displayNameHtml)
                        } else if (root.type === Notification.Update) {
                            return i18n("%1 edited a post", root.notificationActorIdentity.displayNameHtml)
                        } else if (root.type === Notification.Poll) {
                            if (AccountManager.selectedAccount.identity.displayName === root.authorIdentity.displayName) {
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
                text: i18n("Pinned entry")
                color: Kirigami.Theme.disabledTextColor
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }

        RowLayout {
            visible: (root.isBoosted || root.type === Notification.Repeat) && !filtered

            Layout.fillWidth: true
            Layout.bottomMargin: visible ? Kirigami.Units.smallSpacing : 0
            Kirigami.Icon {
                source: "retweet"
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
                        source: root.boostAuthorIdentity && root.boostAuthorIdentity.avatarUrl ? root.boostAuthorIdentity.avatarUrl :  ''
                        cache: true
                        actions.main: Kirigami.Action {
                            tooltip: i18n("View profile")
                            onTriggered: Navigation.openAccount(root.boostAuthorIdentity.id)
                        }
                        name: root.boostAuthorIdentity && root.boostAuthorIdentity.displayName ? root.boostAuthorIdentity.displayName :  ''
                    }
                    QQC2.Label {
                        text: root.boostAuthorIdentity ? i18n("%1 boosted", root.boostAuthorIdentity.displayNameHtml) : (root.type === Notification.Repeat ? i18n("%1 boosted your post", root.notificationActorIdentity.displayNameHtml) : '')
                        color: root.type === Notification.Repeat ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
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
                level: 5
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
                }
                QQC2.Button {
                    text: tootContent.visible ? i18n("Show Less") : i18n("Show More")
                    onClicked: tootContent.visible = !tootContent.visible
                }
            }
            TextEdit {
                id: tootContent
                font.pointSize: heading.font.pointSize
                Layout.fillWidth: true
                text: "<style>
a{
    color: " + Kirigami.Theme.linkColor + ";
    text-decoration: none;
}
</style>" + root.content
                textFormat: TextEdit.RichText
                wrapMode: Text.Wrap
                visible: root.spoilerText.length === 0 || AccountManager.selectedAccount.preferences.extendSpoiler
                readOnly: true
                selectByMouse: !Kirigami.Settings.isMobile
                // TODO handle opening profile page in tokodon
                onLinkActivated: {
                    if (link.startsWith('hashtag:/')) {
                        const item = pageStack.push(tagModelComponent, {
                            hashtag: link.substring(9),
                        })
                    } else {
                        Qt.openUrlExternally(link)
                    }
                }
                color: root.secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
                onHoveredLinkChanged: if (hoveredLink.length > 0) {
                    applicationWindow().hoverLinkIndicator.text = hoveredLink;
                } else {
                    applicationWindow().hoverLinkIndicator.text = "";
                }

                TapHandler {
                    onTapped: root.clicked()
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }

        AttachmentGrid {
            expandedPost: root.expandedPost
            attachments: root.attachments
            sensitive: root.sensitive
            secondary: root.secondary
        }

        LinkPreview {
            attachments: root.attachments
            card: root.card
            secondary: root.secondary
        }

        Poll {
            poll: root.poll
        }

        RowLayout {
            visible: showInteractionButton && !filtered
            InteractionButton {
                iconSource: "reply-post"
                text: root.repliesCount < 2 ? root.repliesCount : (Config.showPostStats || root.expandedPost ? root.repliesCount : i18nc("More than one reply", "1+"))

                onClicked: Navigation.replyTo(root.id, root.mentions, root.visibility, root.authorIdentity)

                QQC2.ToolTip.text: i18nc("Reply to a post", "Reply")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
            InteractionButton {
                iconSource: root.reblogged ? 'boost-post-done' : 'boost-post'
                interacted: root.reblogged
                interactionColor: "green"
                onClicked: timelineModel.actionRepeat(timelineModel.index(root.index, 0))
                text: (Config.showPostStats || root.expandedPost) ? root.reblogsCount : ''
                QQC2.ToolTip.text: i18nc("Share a post", "Boost")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
            InteractionButton {
                iconSource: root.favourited ? 'like-post-done' : 'like-post'
                interacted: root.favourited
                interactionColor: "orange"
                onClicked: timelineModel.actionFavorite(timelineModel.index(root.index, 0))
                text: (Config.showPostStats || root.expandedPost) ? root.favouritesCount : ''
                QQC2.ToolTip.text: i18nc("Like a post", "Like")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
            InteractionButton {
                iconSource: 'bookmarks'
                interacted: root.bookmarked
                interactionColor: "red"
                onClicked: timelineModel.actionBookmark(timelineModel.index(root.index, 0))
                QQC2.ToolTip.text: root.bookmarked ? i18n("Remove bookmark") : i18nc("Bookmark a post", "Bookmark")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
            InteractionButton {
                iconSource: 'overflow-menu'
                onClicked: postMenu.open()

                OverflowMenu {
                    id: postMenu
                    index: root.index
                    postId: root.id
                    url: root.url
                    bookmarked: root.bookmarked
                }

                QQC2.ToolTip.text: i18nc("Show more options", "More")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
        }

        Kirigami.Separator {
            visible: root.showSeparator && !root.selected
            Layout.fillWidth: true
        }
    }
}
