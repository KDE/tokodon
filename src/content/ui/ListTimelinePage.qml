// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon

TimelinePage {
    id: root

    property alias listId: timelineModel.listId
    required property string name
    property Component editListPage: Qt.createComponent("org.kde.tokodon", "EditListPage", Qt.Asynchronous)

    title: name

    showPostAction: false

    Component.onCompleted: {
        // TODO: When we can require KF 6.8, set it as a normal property
        if (root.verticalScrollBarInteractive !== undefined) {
            root.verticalScrollBarInteractive = false;
        }
    }

    actions: Kirigami.Action {
        text: i18n("Edit List")
        icon.name: "edit-rename"
        onTriggered: {
            pageStack.layers.push(editListPage.createObject(root), {
                purpose: EditListPage.Edit,
                listId: root.listId
            });
        }
    }
    model: MainTimelineModel {
        id: timelineModel
        name: "list"
    }
}
