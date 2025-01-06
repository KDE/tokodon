// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later


import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import Qt.labs.qmlmodels
import org.kde.kirigami as Kirigami
import org.kde.tokodon as Tokodon
import org.kde.tokodon.private

NotificationDelegate {
    id: root

    required property Tokodon.annualReportEvent annualReportEvent
    required property string relativeTime

    RowLayout {
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            text: '🎉'
            font.family: 'emoji'
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        }

        ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            QQC2.Label {
                text: xi18nc("@info:label this is used for notifications, when an annual report is available. It's similar to spotify wrapped, it shows profile stats / it's a recap of the year. The variable is the current year e.g. 2024. Please don't translate the hashtag.", "Your %1 #FediWrapped awaits! Unveil your year's highlights and memorable moments on Mastodon!", root.annualReportEvent.year)
                wrapMode: Text.WordWrap

                Layout.fillWidth: true
            }

            QQC2.Button {
                text: i18nc("@action:button", "View #FediWrapped")
                visible: false // TODO implement me
            }
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
