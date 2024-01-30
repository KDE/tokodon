// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.tokodon.private

import "../StatusDelegate" as StatusDelegate

ColumnLayout {
    id: root

    required property var post

    spacing: Kirigami.Units.largeSpacing

    StatusDelegate.InlineIdentityInfo {
        identity: root.post.authorIdentity
        secondary: false

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

        sourceComponent: StatusDelegate.PostContent {
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

        sourceComponent: StatusDelegate.AttachmentGrid {
            expandedPost: false
            attachments: root.post.attachments
            sensitive: false
            secondary: false
            inViewPort: true
            canHideMedia: false
            identity: root.post.authorIdentity
            viewportWidth: root.width
        }
    }

    Loader {
        active: root.post.poll !== null
        sourceComponent: StatusDelegate.StatusPoll
        {
            poll: root.post.poll
        }
    }
}