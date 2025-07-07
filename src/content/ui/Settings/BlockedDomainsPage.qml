// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Window

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.tokodon

Kirigami.ScrollablePage {
    id: root

    title: i18nc("@title", "Blocked Domains")

    function reload(): void {
        model.loadDomains();
    }

    ListView {
        id: listview

        model: BlockedDomainModel {
            id: model
        }
        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property int index
            required property string domain

            text: domain

            contentItem: RowLayout {
                spacing: 0

                QQC2.Label {
                    text: delegate.text
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                QQC2.Button {
                    text: i18nc("@action:button", "Unblock")
                    onClicked: model.actionUnblock(model.index(delegate.index, 0))
                }
            }
        }

        Kirigami.LoadingPlaceholder {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "internet-services-symbolic"
            text: i18nc("@info:placeholder", "No Blocked Domains")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
