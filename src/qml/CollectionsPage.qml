// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.kirigamiaddons.delegates as Delegates

import "./PostDelegate"

Kirigami.ScrollablePage {
    id: root

    property string pageId
    property Component editCollectionPage: Qt.createComponent("org.kde.tokodon", "EditCollectionPage", Qt.Asynchronous)

    function reload(): void {
        model.fillTimeline();
    }

    title: i18nc("@title", "Collections")

    actions: Kirigami.Action {
        text: i18nc("@action:button Create new collection", "Create New…")
        icon.name: "gtk-add"
        onTriggered: {
            const page = pageStack.layers.push(editCollectionPage.createObject(root), {
                purpose: EditCollectionPage.New
            });
            page.done.connect(function(deleted) {
                // Reload the collections since we just added one
                model.fillTimeline();
                pageStack.layers.pop();
            });
        }
    }

    ListView {
        id: listview

        model: CollectionsModel {
            id: model
        }
        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property string id
            required property string name

            text: name

            onClicked: Navigation.openCollection(id, name)
        }

        Kirigami.LoadingPlaceholder {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "view-group"
            text: i18n("No Collections")
            explanation: i18n("Collections allow you to categorize accounts you want other people to follow.")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
