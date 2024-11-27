// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQml.Models
import QtQuick.Layouts

Rectangle {
    id: root

    // The aspect ratio of the viewport.
    readonly property real aspectRatio: width / height

    property alias source: image.source
    property size sourceSize
    property alias status: image.status

    property bool crop: true
    property real focusX: 0.0
    property real focusY: 0.0

    implicitWidth: parent.width
    implicitHeight: parent.width * (9.0 / 16.0)

    clip: true

    Image {
        id: image

        // The aspect ratio of the image (before it's cropped).
        readonly property real aspectRatio: {
            let size = root.sourceSize !== Qt.size(0, 0) ? root.sourceSize : image.sourceSize;
            return size.width / Math.max(size.height, 1);
        }

        // Whether the image is going to be vertically or horizontally cropped, based on if the aspect ratio is bigger
        // or smaller.
        readonly property bool horizontallyCropped: aspectRatio > parent.aspectRatio
        readonly property bool verticallyCropped: aspectRatio < parent.aspectRatio

        // Transforms the focus from [-1, 1] to [0, 1].
        readonly property real focusRangeX: (-root.focusX + 1.0) / 2.0
        readonly property real focusRangeY: (-root.focusY + 1.0) / 2.0

        // The x, y that centers the image.
        readonly property real centerX: parent.width - width
        readonly property real centerY: parent.height - height

        // Sets the position from the range of [0, 1] where (0, 0) is the top-left of the image,
        // and (1, 1) is the bottom-right. Values of (0.5, 0.5) results in a perfectly centered image.
        x: centerX * focusRangeX
        y: centerY * focusRangeY

        // Sets the width and height depending on whether the image is horizontally or vertically cropped.
        // For example, if it's vertically cropped then the image is set to the viewport size and the height is based on
        // the image aspect ratio. Vice versa for horizontally cropped images.
        width: horizontallyCropped ? (parent.height * aspectRatio) : parent.width
        height: verticallyCropped ? (parent.width * (1.0 / aspectRatio)) : parent.height

        cache: false
    }
}
