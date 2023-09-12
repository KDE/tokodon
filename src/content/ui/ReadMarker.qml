// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon

ColumnLayout {
    id: root

    required property var date

    signal clicked()

    QQC2.Label {
        text: i18nc("Last read on this date", "Last read on %1", Qt.formatDate(root.date))
        color: Kirigami.Theme.disabledTextColor

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
    }

    QQC2.Button {
        text: i18nc("@action:button Load more posts above this", "Load More")
        icon.name: "content-loading-symbolic"

        onClicked: root.clicked()

        Layout.alignment: Qt.AlignHCenter
    }
}
