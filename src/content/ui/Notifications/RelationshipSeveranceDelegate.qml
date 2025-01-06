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

    required property var relationshipSeveranceEvent
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
            text: xi18nc("@info:label", "Moderation has blocked <b>%1</b>, including %2 of your followers and %3 account you follow.", root.relationshipSeveranceEvent.targetName, root.relationshipSeveranceEvent.followingCount, root.relationshipSeveranceEvent.followersCount)
            wrapMode: Text.WordWrap

            Layout.fillWidth: true
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
