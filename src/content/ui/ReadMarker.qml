// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.coreaddons as CoreAddons

QQC2.Label {
    id: root

    required property var date

    Layout.alignment: Qt.AlignHCenter

    text: i18nc("Last read on this date", "Last read: %1", CoreAddons.Format.formatRelativeDateTime(root.date, Locale.ShortFormat))
    color: Kirigami.Theme.disabledTextColor
    verticalAlignment: Qt.AlignVCenter
    horizontalAlignment: Qt.AlignHCenter

    Layout.fillWidth: true
    Layout.topMargin: Kirigami.Units.largeSpacing
}
