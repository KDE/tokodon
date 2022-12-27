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

Kirigami.ApplicationWindow {
    id: appwindow

    minimumWidth: Kirigami.Units.gridUnit * 15
    minimumHeight: Kirigami.Units.gridUnit * 20
    pageStack.defaultColumnWidth: Kirigami.Units.gridUnit * 30
    pageStack.globalToolBar.canContainHandles: true
    pageStack.globalToolBar.showNavigationButtons: applicationWindow().pageStack.currentIndex > 0 ? Kirigami.ApplicationHeaderStyle.ShowBackButton : 0

    property bool isShowingFullScreenImage: false

    Connections {
        target: AccountManager

        function onAccountSelected() {
            pageStack.pop(pageStack.get(0))
        }

        function onAccountRemoved() {
            if (!AccountManager.hasAccount) {
                pageStack.replace('qrc:/content/ui/LoginPage.qml');
            }
        }
    }

    globalDrawer: Kirigami.OverlayDrawer {
        id: drawer
        edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
        modal: Kirigami.Settings.isMobile || (applicationWindow().width < Kirigami.Units.gridUnit * 50 && !collapsed) // Only modal when not collapsed, otherwise collapsed won't show.
        z: modal ? Math.round(position * 10000000) : 100
        drawerOpen: !Kirigami.Settings.isMobile
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
        handleVisible: applicationWindow().pageStack.depth <= 1 && applicationWindow().pageStack.layers.depth <= 1 && !isShowingFullScreenImage

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        contentItem: ColumnLayout {
            spacing: 0

            QQC2.ToolBar {
                Layout.fillWidth: true
                Layout.preferredHeight: pageStack.globalToolBar.preferredHeight

                leftPadding: Kirigami.Units.smallSpacing
                rightPadding: Kirigami.Units.smallSpacing

                contentItem: RowLayout {
                    SearchField {
                        Layout.fillWidth: true
                    }
                }
            }

            Repeater {
                model: [homeAction, notificationAction, localTimelineAction, globalTimelineAction, conversationAction]
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
        iconName: "go-home-large"
        text: i18n("Home")
        checkable: true
        checked: true
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace(mainTimeline, {
                type: TimelinePage.TimelineType.Home,
            });
            checked = true;
        }
    }
    property Kirigami.Action notificationAction: Kirigami.Action {
        iconName: "notifications"
        text: i18n("Notifications")
        checkable: true
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace(notificationTimeline);
            checked = true;
        }
    }
    property Kirigami.Action localTimelineAction: Kirigami.Action {
        iconName: "system-users"
        text: i18n("Local")
        checkable: true
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace(mainTimeline, {
                name: "public",
                type: TimelinePage.TimelineType.Local,
            });
            checked = true;
        }
    }
    property Kirigami.Action globalTimelineAction: Kirigami.Action {
        iconName: "kstars_xplanet"
        text: i18n("Global")
        checkable: true
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace(mainTimeline, {
                name: "federated",
                type: TimelinePage.TimelineType.Global,
            });
            checked = true;
        }
    }

    property Kirigami.Action conversationAction: Kirigami.Action {
        iconName: "tokodon-chat-reply"
        text: i18n("Conversation")
        checkable: true
        onTriggered: {
            pageStack.layers.clear();
            pageStack.replace("qrc:/content/ui/ConversationPage.qml");
            checked = true;
        }
    }
    property Kirigami.NavigationTabBar tabBar: Kirigami.NavigationTabBar {
        // Make sure we take in count drawer width
        visible: pageStack.layers.depth <= 1 && AccountManager.hasAccounts && !appwindow.wideScreen
        actions: [homeAction, notificationAction, localTimelineAction, globalTimelineAction, conversationAction]
    }

    footer: Kirigami.Settings.isMobile ? tabBar : null

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    Component.onCompleted: if (AccountManager.hasAccounts) {
        pageStack.push(mainTimeline);
    } else {
        pageStack.push('qrc:/content/ui/LoginPage.qml');
    }

    Component {
        id: mainTimeline
        TimelinePage {
            property alias name: timelineModel.name
            type: TimelinePage.TimelineType.Home
            model: TimelineModel {
                id: timelineModel
                name: "home"
            }
        }
    }

    Component {
        id: notificationTimeline
        NotificationPage {
            property var type
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
            type: TimelinePage.TimelineType.Tag
        }
    }
}
