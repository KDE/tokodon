// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.delegates 1 as Delegates

import org.kde.tokodon

import "./PostDelegate"

Kirigami.ScrollablePage {
    id: root

    title: i18nc("@title Server-wide announcements set by admins.", "Announcements")

    ListView {
        id: listview

        model: AnnouncementModel {}
        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property var index
            required property string id
            required property string content
            required property date publishedAt

            contentItem: ColumnLayout {
                id: layout

                spacing: 0
                clip: true

                Kirigami.Heading {
                    text: i18nc("@label An announcement was published on a date", "Announcement on %1", delegate.publishedAt.toLocaleDateString())
                    type: Kirigami.Heading.Type.Primary
                    level: 4
                    verticalAlignment: Text.AlignTop
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                QQC2.Label {
                    Layout.fillWidth: true

                    text: delegate.content
                    wrapMode: Text.Wrap
                    textFormat: Text.StyledText

                    onLinkActivated: (link) => Qt.openUrlExternally(link)
                }
            }
        }

        Kirigami.LoadingPlaceholder {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "note"
            text: i18nc("@label", "No Announcements")
            explanation: i18n("Your server has not made any announcements yet.")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
