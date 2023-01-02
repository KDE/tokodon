// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

Kirigami.ScrollablePage {
    id: timelinePage

    property var dialog: null

    titleDelegate: Kirigami.Heading {
        // identical to normal Kirigami headers
        Layout.fillWidth: true
        Layout.maximumWidth: implicitWidth + 1
        Layout.minimumWidth: 0
        maximumLineCount: 1
        elide: Text.ElideRight

        text: model.displayName

        textFormat: TextEdit.RichText
    }
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar

    enum TimelineType {
        Home,
        Notification,
        Local,
        Global,
        Thread,
        Profile,
        Tag
    }

    required property var model
    required property var type

    property alias listViewHeader: listview.header
    property bool isProfile: false

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
        model: timelinePage.model

        Component {
            id: fullScreenImage
            FullScreenImage {}
        }
        delegate: PostDelegate {
            timelineModel: timelinePage.model
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("Loading...")
            visible: listview.model.loading && listview.count < 2
        }
    }
}
