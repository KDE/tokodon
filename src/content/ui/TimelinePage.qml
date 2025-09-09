// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigamiaddons.statefulapp as StatefulApp
import org.kde.tokodon
import './PostDelegate'
import './StatusComposer'

Kirigami.ScrollablePage {
    id: root

    property string pageId
    property var dialog: null
    required property var model
    property bool expandedPost: false
    property alias listViewHeader: listview.header
    property bool showPostAction: true
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

        textFormat: Text.RichText
    }

    header: Kirigami.InlineMessage {
        id: message
        type: Kirigami.MessageType.Error
        width: parent.width
        position: Kirigami.InlineMessage.Position.Header

        showCloseButton: true

        actions: Kirigami.Action {
            fromQAction: (root.QQC2.ApplicationWindow.window as StatefulApp.StatefulWindow)?.application.globalActions.action("options_configure") ?? null
        }
    }

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar

    onBackRequested: (event) => {
        if (dialog) {
            dialog.close();
            dialog = null;
            event.accepted = true;
        }
    }

    supportsRefreshing: true
    onRefreshingChanged: {
        if (refreshing) {
            root.model.refresh();
        }
    }

    Connections {
        target: Qt.application
        function onStateChanged(): void {
            // If we're switching back to the application, ensure we refresh the timeline.
            // This only happens on Android because I'm not sure how this would happen on other platforms.
            if (Qt.platform.os == "android" && Application.state == Qt.ApplicationActive) {
                root.model.refresh();
            }
        }
    }

    function returnToTop(): void {
        listview.positionViewAtBeginning();
    }

    actions: [
        Kirigami.Action {
            id: filterAction
            text: i18nc("@action:button", "Filters")
            icon.name: "view-filter"

            Kirigami.Action {
                id: showBoostsAction
                text: i18n("Show Boosts")
                icon.name: "boost"
                checkable: true
                checked: true
            }
            Kirigami.Action {
                id: showRepliesAction
                text: i18n("Show Replies")
                icon.name: "view-conversation-balloon-symbolic"
                checkable: true
                checked: true
            }
            Kirigami.Action {
                separator: true
            }
            Kirigami.Action {
                icon.name: "configure-symbolic"
                text: i18nc("@action:intoolbar", "Configure Filters…")
                onTriggered: pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "FiltersPage"), { title: i18nc("@title:window", "Filters") }, { title: i18nc("@title:window", "Filters") })
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
        target: root.model
        function onNetworkErrorOccurred(error: string): void {
            message.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your settings.", error)
            message.visible = true
        }
        function onLoadingChanged(): void {
            root.refreshing = false;
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
        expandedPost: root.expandedPost
        showPostAction: root.showPostAction

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
                id: flexColumnMarker

                required property bool section

                spacing: 0
                padding: 0
                maximumWidth: Kirigami.Units.gridUnit * 40

                width: parent.width
                height: marker.visible ? Kirigami.Units.gridUnit * 2 : 0

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing
                    visible: flexColumnMarker.section

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
                    visible: flexColumnMarker.section
                    Layout.fillWidth: true
                }
            }
        }
    }
}
