// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import Qt5Compat.GraphicalEffects

// The visual "link preview box" when there's some data attached to a link
// Such as the website page description and title
QQC2.AbstractButton {
    id: root

    required property var post

    leftPadding: Kirigami.Units.largeSpacing
    topPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing

    onClicked: Navigation.openThread(post.postId)

    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        radius: Kirigami.Units.mediumSpacing
        color: Kirigami.Theme.alternateBackgroundColor
        border {
            width: root.visualFocus ? 2 : 0
            color: root.visualFocus ? Kirigami.Theme.focusColor : 'transparent'
        }
    }

    contentItem: RowLayout {
        spacing: 0

        PostContent {
            content: post.content
            expandedPost: false
            secondary: true
            shouldOpenInternalLinks: false

            Layout.fillWidth: true
        }
    }

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }
}
