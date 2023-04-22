// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0
import "./StatusDelegate"
import "./StatusComposer"

Kirigami.ScrollablePage {
    id: timelinePage
    title: i18n("Explore")

    required property var model
    property var dialog: null

    property alias listViewHeader: listview.header

    onBackRequested: if (dialog) {
        dialog.close();
        dialog = null;
        event.accepted = true;
    }

    actions.main: Kirigami.Action {
        icon.name: "list-add"
        text: i18n("Toot")
        enabled: AccountManager.hasAccounts
        onTriggered: {
            const post = AccountManager.selectedAccount.newPost()
            pageStack.layers.push("qrc:/content/ui/TootComposer.qml", {
                postObject: post
            });
        }
    }

    ListView {
        id: listview
        model: LinkPaginationTimelineModel {
            id: trendingPostsModel
            name: "trending"
        }

        Connections {
            target: Navigation
            function onOpenFullScreenImage(attachments, currentIndex) {
                if (timelinePage.isCurrentPage) {
                    timelinePage.dialog = fullScreenImage.createObject(parent, {
                        model: attachments,
                        currentIndex: currentIndex,
                    });
                    timelinePage.dialog.open();
                }
            }
        }

        Connections {
            target: trendingPostsModel
            function onPostSourceReady(backend) {
                pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                    purpose: StatusComposer.Edit,
                    backend: backend
                });
            }
        }

        Component {
            id: fullScreenImage
            FullScreenImage {}
        }
        delegate: StatusDelegate {
            timelineModel: trendingPostsModel
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("Loading...")
            visible: listview.count === 0 && listview.model.loading
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No Posts")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
        }
    }
}
