// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts

// Wrapper component used for the attachment grid
Item {
    id: root

    // The GridLayout holding the media attachments
    required property var gridLayout

    // Whether the media should attempt to limit the aspect ratio to 16:9
    required property bool shouldKeepAspectRatio

    // The width of the AttachmentGrid
    required property real rootWidth

    // The index of this attachment
    required property int index

    // The total number of media attachments
    required property int count

    // Source image dimensions
    required property int sourceWidth
    required property int sourceHeight

    readonly property real aspectRatio: root.sourceHeight / Math.max(root.sourceWidth, 1)
    readonly property real mediaRatio: 9.0 / 16.0

    // If there is three attachments, the first one is bigger than the other two.
    readonly property bool isSpecialAttachment: index == 0 && count == 3

    readonly property int heightDivisor: (isSpecialAttachment || count < 3) ? 1 : 2

    Layout.fillWidth: true
    Layout.fillHeight: !shouldKeepAspectRatio
    Layout.rowSpan: isSpecialAttachment ? 2 : 1

    readonly property real extraSpacing: isSpecialAttachment ? gridLayout.rowSpacing : 0

    Layout.preferredHeight: shouldKeepAspectRatio ? Math.ceil(rootWidth * aspectRatio) : Math.ceil(rootWidth * mediaRatio / heightDivisor) + extraSpacing
}