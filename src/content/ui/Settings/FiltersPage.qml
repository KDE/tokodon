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

    property Component editFilterPage: Qt.createComponent("org.kde.tokodon", "EditFilterPage", Qt.Asynchronous)

    title: i18nc("@title", "Filters")

    function reload(): void {
        model.fillTimeline();
    }

    ListView {
        id: listview

        model: FiltersModel {
            id: model
        }
        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property string id
            required property string title

            text: title

            onClicked: {
                const page = root.Window.window.pageStack.layers.push(editFilterPage.createObject(root), {
                    purpose: EditFilterPage.Edit,
                    filterId: delegate.id
                });
                page.done.connect(function(deleted) {
                    // Reload the filters since we just edited it
                    model.fillTimeline();
                    root.Window.window.pageStack.layers.pop();
                });
            }
        }

        Kirigami.LoadingPlaceholder {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "filter-symbolic"
            text: i18n("No Filters")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
