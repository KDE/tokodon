// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2

QQC2.Label {
    id: root

    required property var date

    text: i18nc("Last read on this date", "Last read on %1", Qt.formatDate(root.date))
    color: Kirigami.Theme.disabledTextColor
    verticalAlignment: Qt.AlignVCenter
    horizontalAlignment: Qt.AlignHCenter
}
