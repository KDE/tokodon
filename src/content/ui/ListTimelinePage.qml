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
    property bool favorite: false

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
            let page = pageStack.layers.push(editListPage.createObject(root), {
                purpose: EditListPage.Edit,
                listId: root.listId
            });
            page.done.connect(function(deleted) {
                pageStack.layers.clear();
                while (pageStack.depth > 1) {
                    pageStack.pop();
                }

                // If we end up on a lists page, make sure to reload the model as we just deleted something.
                const listsPage = (pageStack.currentItem as ListsPage);
                if (listsPage) {
                    listsPage.reload()
                } else {
                    // If we are here because someone has favorited, then we need to go back home.
                    applicationWindow().homeAction.trigger();
                }
            });
        }
    }
    model: MainTimelineModel {
        id: timelineModel
        name: "list"
    }
}
