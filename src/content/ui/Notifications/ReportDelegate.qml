// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import org.kde.tokodon.private

NotificationDelegate {
    id: root

    required property string relativeTime

    RowLayout {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Icon {
            source: 'im-kick-user'
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            color: Kirigami.Theme.disabledTextColor
            Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
            Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
        }

        QQC2.Label {
            text: i18nc("@info:label", "A report has been filed against a user in your server.")
        }

        Item {
            Layout.fillWidth: true
        }

        QQC2.Label {
            text: root.relativeTime
            color: Kirigami.Theme.disabledTextColor
        }
    }
}
