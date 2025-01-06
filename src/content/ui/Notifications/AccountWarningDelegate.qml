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

    required property var notificationActorIdentity
    required property var moderationWarning
    required property string relativeTime

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        RowLayout {
            spacing: Kirigami.Units.smallSpacing

            Layout.topMargin: visible ? Kirigami.Units.smallSpacing : 0
            Layout.bottomMargin: visible ? Kirigami.Units.smallSpacing : 0
            Kirigami.Icon {
                source: 'im-kick-user'
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                color: Kirigami.Theme.disabledTextColor
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }

            QQC2.Label {
                font: Config.defaultFont
                text: i18n("%1 has issued a warning against your account", root.notificationActorIdentity.displayNameHtml)
                textFormat: Text.StyledText
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
            }

            QQC2.Label {
                text: root.relativeTime
                color: Kirigami.Theme.disabledTextColor
            }
        }

        QQC2.Label {
            text: root.moderationWarning.text
        }
    }
}
