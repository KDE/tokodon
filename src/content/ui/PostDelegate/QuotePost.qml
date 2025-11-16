// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import org.kde.tokodon

// The visual "link preview box" when there's some data attached to a link
// Such as the website page description and title
QQC2.AbstractButton {
    id: root

    required property var post

    leftPadding: Kirigami.Units.largeSpacing
    topPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing

    onClicked: Navigation.openPost(post.postId)

    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        radius: Kirigami.Units.cornerRadius
        color: Kirigami.Theme.alternateBackgroundColor
        border {
            width: root.visualFocus ? 2 : 0
            color: root.visualFocus ? Kirigami.Theme.focusColor : 'transparent'
        }
    }

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        RowLayout {
            spacing: Kirigami.Units.smallSpacing

            KirigamiComponents.AvatarButton {
                id: avatar

                source: root.post.authorIdentity.avatarUrl
                cache: true
                onClicked: {
                    Navigation.openAccount(root.post.authorIdentity.id);
                    root.clicked();
                }
                name: root.post.authorIdentity.displayName

                Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
                Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                QQC2.ToolTip.text: i18n("View profile")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }

            QQC2.Label {
                text: i18n("Post from %1", root.post.authorIdentity.displayName)

                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
            }

            Kirigami.Icon {
                implicitWidth: Kirigami.Units.iconSizes.medium
                implicitHeight: Kirigami.Units.iconSizes.medium

                source: "format-text-blockquote-symbolic"

                HoverHandler {
                    id: iconHoverer
                }

                QQC2.ToolTip.text: i18n("Quoted post")
                QQC2.ToolTip.visible: iconHoverer.hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
        }

        ColumnLayout {
            visible: root.post.hasContent
            spacing: Kirigami.Units.largeSpacing

            Layout.fillWidth: true

            Loader {
                visible: root.post.spoilerText.length !== 0
                active: visible

                Layout.fillWidth: true

                sourceComponent: ContentNotice {
                    postContent: postContent
                    spoilerText: root.post.spoilerText

                    // This is intentional as navigating sub-toots is really awkward. Just open it!
                    onToggleNotice: root.clicked()
                }
            }

            PostContent {
                id: postContent

                content: root.post.content
                expandedPost: false
                secondary: root.post.secondary
                visible: root.post.spoilerText.length === 0 || AccountManager.selectedAccount.preferences.extendSpoiler
                shouldOpenInternalLinks: true
            }
        }

        Loader {
            active: root.post.attachments.length > 0 && postContent.visible
            visible: active

            Layout.fillWidth: true

            sourceComponent: AttachmentGrid {
                expandedPost: false
                attachments: root.post.attachments
                sensitive: root.post.sensitive
                secondary: false
                inViewPort: true
                canHideMedia: false
                identity: root.post.authorIdentity
                viewportWidth: root.width
                forceCrop: true
            }
        }

        RowLayout {
            spacing: Kirigami.Units.smallSpacing
            visible: root.post.hasPoll

            Kirigami.Icon {
                source: "amarok_playcount"

                Layout.preferredWidth: Kirigami.Units.iconSizes.sizeForLabels
                Layout.preferredHeight: Kirigami.Units.iconSizes.sizeForLabels
            }

            QQC2.Label {
                text: i18nc("@info:label", "Poll")
                color: Kirigami.Theme.disabledTextColor
            }
        }
    }

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }
}
