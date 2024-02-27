// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.kirigamiaddons.delegates 1 as Delegates

import "./PostDelegate"

Kirigami.ScrollablePage {
    id: root

    title: i18nc("@title", "Lists")
    titleDelegate: Kirigami.Heading {
        // identical to normal Kirigami headers
        Layout.fillWidth: true
        Layout.maximumWidth: implicitWidth + 1
        Layout.minimumWidth: 0
        maximumLineCount: 1
        elide: Text.ElideRight
        text: root.title
        textFormat: TextEdit.RichText
    }

    actions: Kirigami.Action {
        text: i18n("Create List")
        icon.name: "gtk-add"
        onTriggered: {
            pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "EditListPage"), {
                purpose: EditListPage.New
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

        QQC2.ProgressBar {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
            indeterminate: true
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No lists")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}