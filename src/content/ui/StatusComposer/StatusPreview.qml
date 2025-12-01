// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.tokodon


import "../PostDelegate" as PostDelegate

ColumnLayout {
    id: root

    required property var post

    spacing: Kirigami.Units.largeSpacing

    InlineIdentityInfo {
        identity: root.post.authorIdentity

        Kirigami.Heading {
            id: heading
            font.pixelSize: Config.defaultFont.pixelSize + 1
            text: root.post.relativeTime
            verticalAlignment: Text.AlignVCenter
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: true
            elide: Text.ElideRight
        }
    }

    Loader {
        active: post.content.length > 0
        visible: active

        Layout.fillWidth: true

        sourceComponent: PostDelegate.PostContent {
            content: post.content
            expandedPost: false
            secondary: true
            shouldOpenInternalLinks: false
        }
    }

    Loader {
        active: post.attachments.length > 0
        visible: active

        Layout.fillWidth: true

        sourceComponent: PostDelegate.AttachmentGrid {
            expandedPost: false
            attachments: root.post.attachments
            sensitive: false
            secondary: false
            inViewPort: true
            canHideMedia: false
            identity: root.post.authorIdentity
            viewportWidth: root.width
            forceCrop: true
        }
    }

    Loader {
        active: root.post.poll !== null
        sourceComponent: PostDelegate.PostPoll {
            poll: root.post.poll
        }
    }
}
