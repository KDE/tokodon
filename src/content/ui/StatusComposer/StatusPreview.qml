// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kmasto

import "../StatusDelegate" as StatusDelegate

ColumnLayout {
    id: root

    required property var post

    StatusDelegate.InlineIdentityInfo {
        identity: root.post.authorIdentity
        secondary: false

        Kirigami.Heading {
            id: heading
            font.pixelSize: Config.defaultFont.pixelSize + 1
            text: root.post.relativeTime
            verticalAlignment: Text.AlignTop
            Layout.alignment: Qt.AlignTop
            elide: Text.ElideRight
        }
    }

    StatusDelegate.PostContent {
        content: post.content
        expandedPost: false
        secondary: true
        shouldOpenInternalLinks: false
    }

    StatusDelegate.AttachmentGrid {
        expandedPost: false
        attachments: root.post.attachments
        sensitive: false
        secondary: false
        inViewPort: true
        canHideMedia: false
        identity: root.post.authorIdentity
    }

    Loader {
        active: root.post.poll !== null
        sourceComponent: StatusDelegate.StatusPoll
        {
            poll: root.post.poll
        }
    }
}