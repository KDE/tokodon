// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import './StatusDelegate'
import './StatusComposer'

Kirigami.ScrollablePage {
    id: root

    property var dialog: null
    required property var model
    property bool expandedPost: false
    property alias listViewHeader: listview.header

    title: model.displayName
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

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar

    onBackRequested: if (dialog) {
        dialog.close();
        dialog = null;
        event.accepted = true;
    }

    actions.main: Kirigami.Action {
        icon.name: "list-add"
        text: i18n("Post")
        enabled: AccountManager.hasAccounts
        onTriggered: Navigation.openStatusComposer()
    }

    Connections {
        target: Navigation
        function onOpenFullScreenImage(attachments, currentIndex) {
            if (root.isCurrentPage) {
                root.dialog = fullScreenImage.createObject(parent, {
                    model: attachments,
                    currentIndex: currentIndex,
                });
                root.dialog.open();
            }
        }
    }

    Connections {
        target: Controller
        function onNetworkErrorOccurred(error) {
            message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your settings.", error)
            message.visible = true
        }
    }

    Connections {
        target: root.model
        function onPostSourceReady(backend, isEdit) {
            pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                purpose: isEdit ? StatusComposer.Edit : StatusComposer.Redraft,
                backend: backend
            });
        }
    }

    ListView {
        id: listview
        model: root.model

        Component {
            id: fullScreenImage
            FullScreenImage {}
        }

        delegate: StatusDelegate {
            timelineModel: root.model
            expandedPost: root.expandedPost
            showSeparator: index !== ListView.view.count - 1
            inViewPort: root.Kirigami.ColumnView.inViewport
            Layout.fillWidth: true
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("Loading...")
            visible: listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No posts")
            visible: !listview.model.loading && listview.count === 0
            width: parent.width - Kirigami.Units.gridUnit * 4
        }

        Kirigami.InlineMessage {
            id: message
            type: Kirigami.MessageType.Error
            anchors {
                top: parent.top
                topMargin: Kirigami.Units.largeSpacing
                left: parent.left
                leftMargin: Kirigami.Units.largeSpacing
                right: parent.right
                rightMargin: Kirigami.Units.largeSpacing
            }

            showCloseButton: true

            actions: Kirigami.Action {
                text: i18n("Settings")
                icon.name: "settings-configure"
                onTriggered: pageStack.pushDialogLayer('qrc:/content/ui/Settings/SettingsPage.qml', {}, { title: i18n("Configure") })
            }
        }
    }
}
