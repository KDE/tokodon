// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import org.kde.kmasto 1.0

import "./StatusComposer"

Kirigami.ApplicationWindow {
    id: appwindow

    minimumWidth: Kirigami.Units.gridUnit * 15
    minimumHeight: Kirigami.Units.gridUnit * 20

    pageStack {
        defaultColumnWidth: Kirigami.Units.gridUnit * 30

        globalToolBar {
            canContainHandles: true
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: applicationWindow().pageStack.currentIndex > 0 ? Kirigami.ApplicationHeaderStyle.ShowBackButton : 0
        }
    }

    property bool isShowingFullScreenImage: false

    Connections {
        target: AccountManager

        function onAccountSelected() {
            pageStack.pop(pageStack.get(0))
        }

        function onAccountRemoved() {
            if (!AccountManager.hasAccounts) {
                pageStack.replace('qrc:/content/ui/LoginPage.qml');
                globalDrawer.drawerOpen = false
            }
        }
    }

    Connections {
        target: Navigation

        function onOpenStatusComposer() {
            pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                purpose: StatusComposer.New
            });
        }

        function onReplyTo(inReplyTo, mentions, visibility, authorIdentity) {
            if (!mentions.includes(`@${authorIdentity.account}`)) {
                mentions.push(`@${authorIdentity.account}`);
            }
            pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                purpose: StatusComposer.Reply,
                inReplyTo: inReplyTo,
                mentions: mentions,
                visibility: visibility,
            });
        }

        function onOpenThread(postId) {
            if (!pageStack.currentItem.postId || pageStack.currentItem.postId !== postId) {
                pageStack.push("qrc:/content/ui/ThreadPage.qml", {
                    postId: postId,
                });
            }
        }

        function onOpenAccount(accountId) {
            if (!pageStack.currentItem.accountId || pageStack.currentItem.accountId !== accountId) {
                pageStack.push('qrc:/content/ui/AccountInfo.qml', {
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
        enabled: AccountManager.hasAccounts
        edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
        modal: !enabled || Kirigami.Settings.isMobile || (applicationWindow().width < Kirigami.Units.gridUnit * 50 && !collapsed) // Only modal when not collapsed, otherwise collapsed won't show.
        z: modal ? Math.round(position * 10000000) : 100
        drawerOpen: !Kirigami.Settings.isMobile && enabled
        width: Kirigami.Units.gridUnit * 16
        Behavior on width {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        handleClosedIcon.source: modal ? null : "sidebar-expand-left"
        handleOpenIcon.source: modal ? null : "sidebar-collapse-left"
        handleVisible: modal && !isShowingFullScreenImage && enabled
        onModalChanged: if (!modal) {
            drawerOpen = true;
        }

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        contentItem: ColumnLayout {
            spacing: 0

            QQC2.ToolBar {
                Layout.fillWidth: true
                Layout.preferredHeight: pageStack.globalToolBar.preferredHeight

                leftPadding: 3
                rightPadding: 3
                topPadding: 3
                bottomPadding: 3

                contentItem: SearchField {}
            }

            Repeater {
                model: [homeAction, notificationAction, localTimelineAction, globalTimelineAction, conversationAction, favouritesAction, bookmarksAction]
                Kirigami.BasicListItem {
                    action: modelData
                    separatorVisible: false
                }
            }

            Item {
                Layout.fillHeight: true
            }

            UserInfo {
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
            pageStack.layers.clear();
            pageStack.replace(mainTimeline, {
                name: "home"
            });
            checked = true;
        }
    }
    property Kirigami.Action notificationAction: Kirigami.Action {
        icon.name: "notifications"
        text: i18n("Notifications")
        checkable: true
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace(notificationTimeline);
            checked = true;
        }
    }
    property Kirigami.Action localTimelineAction: Kirigami.Action {
        icon.name: "system-users"
        text: i18n("Local")
        checkable: true
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace(mainTimeline, {
                name: "public",
            });
            checked = true;
        }
    }
    property Kirigami.Action globalTimelineAction: Kirigami.Action {
        icon.name: "kstars_xplanet"
        text: i18n("Global")
        checkable: true
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace(mainTimeline, {
                name: "federated",
            });
            checked = true;
        }
    }

    property Kirigami.Action conversationAction: Kirigami.Action {
        icon.name: "tokodon-chat-reply"
        text: i18n("Conversation")
        checkable: true
        visible: !Kirigami.Settings.isMobile
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace("qrc:/content/ui/ConversationPage.qml");
            checked = true;
        }
    }

    property Kirigami.Action favouritesAction: Kirigami.Action {
        icon.name: "favorite"
        text: i18n("Favourites")
        checkable: true
        visible: !Kirigami.Settings.isMobile
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace(linkPaginationTimeline, {
                name: "favourites",
            });
            checked = true;
        }
    }

    property Kirigami.Action bookmarksAction: Kirigami.Action {
        icon.name: "bookmarks"
        text: i18n("Bookmarks")
        checkable: true
        visible: !Kirigami.Settings.isMobile
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace(linkPaginationTimeline, {
                name: "bookmarks",
            });
            checked = true;
        }
    }

    property Kirigami.NavigationTabBar tabBar: Kirigami.NavigationTabBar {
        // Make sure we take in count drawer width
        visible: pageStack.layers.depth <= 1 && AccountManager.hasAccounts && !appwindow.wideScreen
        actions: [homeAction, notificationAction, localTimelineAction, globalTimelineAction, conversationAction, favouritesAction, bookmarksAction]
    }

    footer: Kirigami.Settings.isMobile ? tabBar : null

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    Component.onCompleted: if (AccountManager.hasAccounts) {
        pageStack.push(mainTimeline, {
            name: 'home',
        });
    } else {
        pageStack.push('qrc:/content/ui/LoginPage.qml');
    }

    Component {
        id: mainTimeline
        TimelinePage {
            id: timelinePage
            property string name
            model: TimelineModel {
                id: timelineModel
                name: timelinePage.name
            }
        }
    }

    Component {
        id: linkPaginationTimeline
        TimelinePage {
            id: timelinePage
            property string name
            model: LinkPaginationTimelineModel {
                id: timelineModel
                name: timelinePage.name
            }
        }
    }

    Component {
        id: notificationTimeline
        NotificationPage {
            model: TimelineModel {
                id: timelineModel
            }
        }
    }

    property Item hoverLinkIndicator: QQC2.Control {
        parent: overlay.parent 
        property alias text: linkText.text
        opacity: text.length > 0 ? 1 : 0
        visible: !Kirigami.Settings.isMobile && !text.startsWith("hashtag:")

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
            model: TagsModel {
                hashtag: tagPage.hashtag
            }
        }
    }
}
