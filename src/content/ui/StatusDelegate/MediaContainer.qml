// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import QtGraphicalEffects 1.0
import Qt.labs.qmlmodels 1.0
import QtMultimedia 5.15

QQC2.Control {
    id: root

    required property var repeater
    required property var aspectRatio
    required property var shouldKeepAspectRatio
    required property var mediaRatio
    required property var rootWidth
    required property var gridLayout

    property var isSpecialAttachment: index == 0 && repeater.count == 3
    property var widthDivisor: repeater.count > 1 ? 2 : 1

    // the first attachment in a three attachment set is displayed at full height
    property var heightDivisor: isSpecialAttachment ? 1 : (repeater.count > 2 ? 2 : 1)
    Layout.rowSpan: isSpecialAttachment ? 2 : 1

    Layout.fillWidth: shouldKeepAspectRatio
    Layout.fillHeight: shouldKeepAspectRatio

    Layout.preferredWidth: shouldKeepAspectRatio ? -1 : parent.width / widthDivisor
    Layout.preferredHeight: shouldKeepAspectRatio ? parent.width * aspectRatio : (rootWidth * mediaRatio) / heightDivisor + (isSpecialAttachment ? gridLayout.rowSpacing : 0)
}