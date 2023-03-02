// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

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
    }

    StatusDelegate.LinkPreview {
        attachments: root.post.attachments
        card: root.post.card
        secondary: true
        contentVisible: true
    }

    StatusDelegate.Poll {
        poll: root.post.poll
    }
}