// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.kirigamiaddons.delegates as Delegates

import "./PostDelegate"

Kirigami.ScrollablePage {
    id: root

    property string pageId
    property Component editListPage: Qt.createComponent("org.kde.tokodon", "EditListPage", Qt.Asynchronous)

    function reload(): void {
        model.fillTimeline();
    }

    title: KI18n.i18nc("@title", "Lists")
    titleDelegate: Kirigami.Heading {
        // identical to normal Kirigami headers
        Layout.fillWidth: true
        Layout.maximumWidth: implicitWidth + 1
        Layout.minimumWidth: 0
        maximumLineCount: 1
        elide: Text.ElideRight
        text: root.title
        textFormat: Text.RichText
    }

    actions: Kirigami.Action {
        text: KI18n.i18nc("@action:button Create new list", "Create New…")
        icon.name: "gtk-add"
        onTriggered: {
            const page = pageStack.layers.push(editListPage.createObject(root), {
                purpose: EditListPage.New
            });
            page.done.connect(function(deleted) {
                // Reload the lists since we just added one
                model.fillTimeline();
                pageStack.layers.pop();
            });
        }
    }

    ListView {
        id: listview

        model: ListsModel {
            id: model
        }
        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property string id
            required property string title

            text: title

            onClicked: Navigation.openList(id, title)
        }

        Kirigami.LoadingPlaceholder {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "view-list-text"
            text: KI18n.i18n("No Lists")
            explanation: KI18n.i18n("Lists allow you to categorize who you're following.")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
