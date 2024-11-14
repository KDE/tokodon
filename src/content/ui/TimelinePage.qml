// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.components 1 as Components
import org.kde.kirigamiaddons.statefulapp as StatefulApp
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import './PostDelegate'
import './StatusComposer'

Kirigami.ScrollablePage {
    id: root

    property var dialog: null
    required property var model
    property bool expandedPost: false
    property alias listViewHeader: listview.header
    property alias showPostAction: postAction.visible
    property bool completedInitialLoad: false
    property alias listView: listview
    readonly property bool showReplies: showRepliesAction.checked
    readonly property bool showBoosts: showBoostsAction.checked
    property alias showFilterAction: filterAction.visible
    property alias originalPostUrl: listview.originalPostUrl
    property string iconName
    property alias placeholderText: placeholderMessage.text
    property alias placeholderExplanation: placeholderMessage.explanation

    Keys.onPressed: event => listview.handleKeyEvent(event)

    title: {
        // Show the account name if the drawer is not open, so there's no way to tell which account you're on.
        if (model.name === "home" && !applicationWindow().globalDrawer.drawerOpen) {
            if (AccountManager.rowCount() > 1) {
                if (AccountManager.selectedAccount === null) {
                    return i18n("Loading");
                }
                return i18n("Home (%1)", AccountManager.selectedAccount.identity.displayNameHtml);
            }
        }

        return model.displayName ?? "";
    }
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

    header: Kirigami.InlineMessage {
        id: message
        type: Kirigami.MessageType.Error
        width: parent.width
        position: Kirigami.InlineMessage.Position.Header

        showCloseButton: true

        actions: Kirigami.Action {
            fromQAction: (root.QQC2.ApplicationWindow.window as StatefulApp.StatefulWindow)?.application.action('options_configure') ?? null
        }
    }

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar

    onBackRequested: if (dialog) {
        dialog.close();
        dialog = null;
        event.accepted = true;
    }

    actions: [
        Kirigami.Action {
            id: postAction
            icon.name: "list-add"
            text: i18nc("@action:button", "Post")
            enabled: AccountManager.hasAccounts
            onTriggered: Navigation.openComposer("")
        },
        Kirigami.Action {
            id: filterAction
            text: i18nc("@action:button", "Filters")
            icon.name: "view-filter"

            Kirigami.Action {
                id: showBoostsAction
                text: i18n("Show Boosts")
                icon.name: "tokodon-post-boost"
                checkable: true
                checked: true
            }
            Kirigami.Action {
                id: showRepliesAction
                text: i18n("Show Replies")
                icon.name: "tokodon-post-reply"
                checkable: true
                checked: true
            }
        }
    ]

    Component.onCompleted: {
        // TODO: When we can require KF 6.8, set it as a normal property
        if (root.verticalScrollBarInteractive !== undefined) {
            root.verticalScrollBarInteractive = false;
        }
    }

    Connections {
        target: Controller
        function onNetworkErrorOccurred(error) {
            message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your settings.", error)
            message.visible = true
        }
    }

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor

        Kirigami.PlaceholderMessage {
            id: placeholderMessage

            anchors.centerIn: parent
            icon.name: root.iconName
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }

    TimelineView {
        id: listview

        model: root.model
        isCurrentPage: root.isCurrentPage

        header: Kirigami.FlexColumn {
            id: flexColumn

            spacing: 0
            padding: 0
            maximumWidth: Kirigami.Units.gridUnit * 40

            width: parent.width
            height: visible ? Kirigami.Units.gridUnit * 3 : 0
            visible: root.model.hasPrevious ?? false

            QQC2.Button {
                text: i18nc("@action:button Load more posts above this", "Load More")
                icon.name: "content-loading-symbolic"

                onClicked: timelineModel.fetchPrevious()

                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: Kirigami.Units.largeSpacing
            }
        }

        section {
            property: "showReadMarker"
            delegate: Kirigami.FlexColumn {
                id: flexColumn

                required property bool section

                spacing: 0
                padding: 0
                maximumWidth: Kirigami.Units.gridUnit * 40

                width: parent.width
                height: marker.visible ? Kirigami.Units.gridUnit * 2 : 0

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing
                    visible: flexColumn.section

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    Layout.bottomMargin: Kirigami.Units.largeSpacing

                    Item {
                        Layout.fillWidth: true
                    }

                    Kirigami.Icon {
                        source: "view-readermode"
                        color: Kirigami.Theme.disabledTextColor

                        Layout.preferredWidth: Kirigami.Units.iconSizes.sizeForLabels
                        Layout.preferredHeight: Kirigami.Units.iconSizes.sizeForLabels
                    }

                    ReadMarker {
                        id: marker

                        date: root.model.lastReadTime
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                Kirigami.Separator {
                    visible: flexColumn.section
                    Layout.fillWidth: true
                }
            }
        }
    }
}
