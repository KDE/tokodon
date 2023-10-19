// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models
import org.kde.tokodon
import org.kde.kirigamiaddons.delegates 1 as Delegates

import "./StatusComposer"
import "./StatusDelegate"

Kirigami.ApplicationWindow {
    id: appwindow

    property bool isShowingFullScreenImage: false

    minimumWidth: Kirigami.Units.gridUnit * 15
    minimumHeight: Kirigami.Units.gridUnit * 20

    pageStack {
        defaultColumnWidth: appwindow.width

        globalToolBar {
            canContainHandles: true
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: if (applicationWindow().pageStack.currentIndex > 0
                || applicationWindow().pageStack.currentIndex > 0) {
                Kirigami.ApplicationHeaderStyle.ShowBackButton
            } else {
                0
            }
        }
    }

    function decideDefaultPage() {
        pageStack.clear();

        if (AccountManager.selectedAccountHasIssue) {
            pageStack.push(Qt.createComponent("org.kde.tokodon", "LoginIssuePage"));
        } else {
            pageStack.push(mainTimeline, {
                name: 'home',
            });
        }
    }

    function startupAccountCheck() {
        if (AccountManager.hasAccounts) {
            decideDefaultPage();
        } else {
            pageStack.push(Qt.createComponent("org.kde.tokodon", "WelcomePage"));
        }
    }

    function navigateLink(link, shouldOpenInternalLinks) {
        if (link.startsWith('hashtag:/') && shouldOpenInternalLinks) {
            pageStack.push(tagModelComponent, {
                hashtag: link.substring(9),
            })
        } else if (link.startsWith('account:/') && shouldOpenInternalLinks) {
            Navigation.openAccount(link.substring(9))
        } else if (link.startsWith('web+ap:/') && shouldOpenInternalLinks) {
            Controller.openWebApLink(link.substring(8))
        } else {
            Qt.openUrlExternally(link)
        }
    }

    Component.onCompleted: {
        if (AccountManager.isReady) {
            startupAccountCheck();
        }

        saveWindowGeometryConnections.enabled = true;
    }

    Connections {
        target: AccountManager

        function onAccountSelected() {
            decideDefaultPage();
        }

        function onAccountRemoved() {
            if (!AccountManager.hasAccounts) {
                pageStack.replace(Qt.createComponent("org.kde.tokodon", "WelcomePage"));
                globalDrawer.drawerOpen = false
            }
        }

        function onAccountsReloaded() {
            pageStack.replace(mainTimeline, {
                name: "home"
            });
        }

        function onAccountsReady() {
            appwindow.startupAccountCheck();
        }
    }

    Connections {
        target: Controller

        function onOpenPost(id) {
            Navigation.openThread(id)
        }

        function onOpenAccount(id) {
            Navigation.openAccount(id)
        }

        function onOpenComposer(text) {
            pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                purpose: StatusComposer.New,
                initialText: text,
                onPostFinished: applicationWindow().pageStack.layers.pop()
            });
        }
    }

    Connections {
        target: Navigation

        function onOpenStatusComposer() {
            pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                purpose: StatusComposer.New
            });
        }

        function onReplyTo(inReplyTo, mentions, visibility, authorIdentity, post) {
            if (!mentions.includes(`@${authorIdentity.account}`)) {
                mentions.push(`@${authorIdentity.account}`);
            }
            pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                purpose: StatusComposer.Reply,
                inReplyTo: inReplyTo,
                mentions: mentions,
                visibility: visibility,
                previewPost: post
            });
        }

        function onOpenThread(postId) {
            if (!pageStack.currentItem.postId || pageStack.currentItem.postId !== postId) {
                pageStack.push(Qt.createComponent("org.kde.tokodon", "ThreadPage"), {
                    postId: postId,
                });
            }
        }

        function onOpenAccount(accountId) {
            if (!pageStack.currentItem.accountId || pageStack.currentItem.accountId !== accountId) {
                pageStack.push(Qt.createComponent("org.kde.tokodon", "AccountInfo"), {
                    accountId: accountId,
                });
            }
        }

        function onOpenTag(tag) {
            pageStack.push(tagModelComponent, {
                hashtag: tag,
            })
        }
    }

    globalDrawer: Kirigami.OverlayDrawer {
        id: drawer
        enabled: AccountManager.hasAccounts && AccountManager.isReady
        edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
        modal: !enabled || Kirigami.Settings.isMobile || Kirigami.Settings.tabletMode || (applicationWindow().width < Kirigami.Units.gridUnit * 50 && !collapsed) // Only modal when not collapsed, otherwise collapsed won't show.
        z: modal ? Math.round(position * 10000000) : 100
        drawerOpen: !Kirigami.Settings.isMobile && enabled
        width: Kirigami.Units.gridUnit * 16
        Behavior on width {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false

        handleClosedIcon.source: modal ? null : "sidebar-expand-left"
        handleOpenIcon.source: modal ? null : "sidebar-collapse-left"
        handleVisible: modal && !isShowingFullScreenImage && enabled
        onModalChanged: drawerOpen = !modal;

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        contentItem: ColumnLayout {
            spacing: 0

            QQC2.ToolBar {
                Layout.fillWidth: true
                Layout.preferredHeight: pageStack.globalToolBar.preferredHeight
                Layout.bottomMargin: Kirigami.Units.smallSpacing / 2

                leftPadding: 3
                rightPadding: 3
                topPadding: 3
                bottomPadding: 3

                visible: !Kirigami.Settings.isMobile

                contentItem: SearchField {}
            }

            UserInfo {
                Layout.fillWidth: true
            }

            Kirigami.Separator {
                Layout.fillWidth: true
                Layout.margins: Kirigami.Units.smallSpacing
            }

            QQC2.ButtonGroup {
                id: pageButtonGroup
            }

            Repeater {
                model: [homeAction, notificationAction, searchAction, announcementsAction, followRequestAction, localTimelineAction, globalTimelineAction, exploreAction, conversationAction, favouritesAction, bookmarksAction]
                Delegates.RoundedItemDelegate {
                    required property var modelData
                    QQC2.ButtonGroup.group: pageButtonGroup

                    padding: Kirigami.Units.largeSpacing
                    action: modelData
                    Layout.fillWidth: true
                    visible: modelData.visible
                    enabled: !AccountManager.selectedAccountHasIssue
                }
            }

            Item {
                Layout.fillHeight: true
            }

            Delegates.RoundedItemDelegate {
                icon.name: "lock"
                onClicked: pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "ModerationToolPage"), {}, {title: i18n("Moderation Tools")})
                text: i18nc("@action:button Open moderation tools", "Moderation Tools")
                visible: AccountManager.selectedAccount && (AccountManager.selectedAccount.identity.permission & AdminAccountInfo.ManageUsers)
                padding: Kirigami.Units.largeSpacing

                Layout.fillWidth: true
            }

            Delegates.RoundedItemDelegate {
                icon.name: "settings-configure"
                onClicked: pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "SettingsPage"), {}, {title: i18n("Configure")})
                text: i18nc("@action:button Open settings dialog", "Settings")
                padding: Kirigami.Units.largeSpacing

                Layout.fillWidth: true
            }
        }
    }

    property Kirigami.Action homeAction: Kirigami.Action {
        icon.name: "go-home-large"
        text: i18n("Home")
        checkable: true
        checked: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(mainTimeline, {
                name: "home"
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }
    property Kirigami.Action notificationAction: Kirigami.Action {
        icon.name: "notifications"
        text: i18n("Notifications")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(notificationTimeline);
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }
    property Kirigami.Action followRequestAction: Kirigami.Action {
        icon.name: "list-add-user"
        text: i18n("Follow Requests")
        checkable: true
        visible: AccountManager.hasAccounts && AccountManager.selectedAccount && AccountManager.selectedAccount.hasFollowRequests
        onTriggered: {
            pageStack.clear();
            pageStack.push(socialGraphComponent, {
                name: "request"
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }
    property Kirigami.Action localTimelineAction: Kirigami.Action {
        icon.name: "system-users"
        text: i18n("Local")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(mainTimeline, {
                name: "public",
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }
    property Kirigami.Action globalTimelineAction: Kirigami.Action {
        icon.name: "kstars_xplanet"
        text: i18n("Global")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(mainTimeline, {
                name: "federated",
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action conversationAction: Kirigami.Action {
        icon.name: "tokodon-chat-reply"
        text: i18n("Conversation")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(Qt.createComponent("org.kde.tokodon", "ConversationPage"));
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action favouritesAction: Kirigami.Action {
        icon.name: "favorite"
        text: i18n("Favourites")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(mainTimeline, {
                name: "favourites",
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action bookmarksAction: Kirigami.Action {
        icon.name: "bookmarks"
        text: i18n("Bookmarks")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(mainTimeline, {
                name: "bookmarks",
            });
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action exploreAction: Kirigami.Action {
        icon.name: "kstars_planets"
        text: i18n("Explore")
        checkable: true
        onTriggered: {
            pageStack.clear();
            pageStack.push(exploreTimeline);
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action searchAction: Kirigami.Action {
        icon.name: "search"
        text: i18n("Search")
        checkable: true
        visible: Kirigami.Settings.isMobile
        onTriggered: {
            pageStack.clear();
            pageStack.push(Qt.createComponent("org.kde.tokodon", "SearchPage"));
            checked = true;
            if (Kirigami.Settings.isMobile) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.Action announcementsAction: Kirigami.Action {
        icon.name: "note"
        text: i18nc("@action:button Server Announcements", "Announcements")
        checkable: true
        visible: AccountManager.hasAccounts && AccountManager.selectedAccount
        onTriggered: {
            pageStack.clear();
            pageStack.push(Qt.createComponent("org.kde.tokodon", "AnnouncementsPage"));
            checked = true;
            if (Kirigami.Settings.isMobile || drawer.modal) {
                drawer.drawerOpen = false;
            }
        }
    }

    property Kirigami.NavigationTabBar tabBar: Kirigami.NavigationTabBar {
        // Make sure we take in count drawer width
        visible: pageStack.layers.depth <= 1 && AccountManager.hasAccounts && !appwindow.wideScreen
        actions: [homeAction, notificationAction, localTimelineAction, globalTimelineAction]
        enabled: !AccountManager.selectedAccountHasIssue
    }

    footer: Kirigami.Settings.isMobile ? tabBar : null

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    Component {
        id: mainTimeline
        TimelinePage {
            id: timelinePage
            property string name
            model: MainTimelineModel {
                id: timelineModel
                name: timelinePage.name
            }
        }
    }

    Component {
        id: socialGraphComponent
        SocialGraphPage {
            id: socialGraphPage
            property alias name: socialGraphModel.name
            property alias accountId: socialGraphModel.accountId
            property alias statusId: socialGraphModel.statusId
            property alias count: socialGraphModel.count
            model: SocialGraphModel {
                id: socialGraphModel
                name: socialGraphPage.name
                accountId: socialGraphPage.accountId
                statusId: socialGraphPage.statusId
                count: socialGraphPage.count
            }
        }
    }

    Component {
        id: notificationTimeline
        NotificationPage { }
    }

    Component {
        id: exploreTimeline
        ExplorePage { }
    }

    property Item hoverLinkIndicator: QQC2.Control {
        parent: overlay.parent
        property alias text: linkText.text
        opacity: text.length > 0 ? 1 : 0
        visible: !Kirigami.Settings.isMobile && !text.startsWith("hashtag:") && !text.startsWith("account:")

        z: 999990
        x: 0
        y: parent.height - implicitHeight
        contentItem: QQC2.Label {
            id: linkText
        }
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        background: Rectangle {
             color: Kirigami.Theme.backgroundColor
        }
    }

    Component {
        id: tagModelComponent
        TimelinePage {
            id: tagPage
            property string hashtag
            model: TagsTimelineModel {
                hashtag: tagPage.hashtag
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        visible: !AccountManager.isReady
        color: Kirigami.Theme.backgroundColor

        Kirigami.LoadingPlaceholder {
            anchors.centerIn: parent
        }
    }

    // This timer allows to batch update the window size change to reduce
    // the io load and also work around the fact that x/y/width/height are
    // changed when loading the page and overwrite the saved geometry from
    // the previous session.
    Timer {
        id: saveWindowGeometryTimer
        interval: 1000
        onTriggered: WindowController.saveGeometry()
    }

    Connections {
        id: saveWindowGeometryConnections
        enabled: false // Disable on startup to avoid writing wrong values if the window is hidden
        target: root

        function onClosing() { WindowController.saveGeometry(); }
        function onWidthChanged() { saveWindowGeometryTimer.restart(); }
        function onHeightChanged() { saveWindowGeometryTimer.restart(); }
        function onXChanged() { saveWindowGeometryTimer.restart(); }
        function onYChanged() { saveWindowGeometryTimer.restart(); }
    }
}
