// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

Kirigami.ScrollablePage {
    id: timelinePage
    title: model.displayName

    required property var model

    property alias listViewHeader: listview.header
    property bool isProfile: false

    supportsRefreshing: true

    onRefreshingChanged: {
        if (refreshing) {
            model.refresh();
        }
    }

    Connections {
        target: model

        function onFetchingChanged() {
            timelinePage.refreshing = false
        }
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
        model: timelinePage.model

        Component {
            id: fullScreenImage
            FullScreenImage {}
        }
        delegate: PostDelegate {}

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("Loading...")
            visible: listview.count === 0 && listview.model.loading
        }
    }
}
