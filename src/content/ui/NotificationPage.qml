// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import "./PostDelegate"
import "./StatusComposer"

Kirigami.ScrollablePage {
    id: timelinePage
    title: i18n("Notifications")

    property var dialog: null

    property alias listViewHeader: listview.header
    readonly property bool typesAreGroupable: showAllAction.checked
    property bool shouldGroupNotifications: typesAreGroupable
    readonly property var currentModel: shouldGroupNotifications ? groupedNotificationModel : notificationModel

    actions: [
        Kirigami.Action {
            icon.name: "checkmark-symbolic"
            text: i18nc("@action:intoolbar Mark all notifications as read", "Mark All As Read")
            enabled: AccountManager.selectedAccount.unreadNotificationsCount > 0
            onTriggered: timelinePage.currentModel.markAllNotificationsAsRead()
        },
        Kirigami.Action {
            icon.name: "configure-symbolic"
            text: i18nc("@action:intoolbar", "Configure Notifications…")
            onTriggered: pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "NotificationsPage"))
        }
    ]

    onBackRequested: if (dialog) {
        dialog.close();
        dialog = null;
        event.accepted = true;
    }

    property Kirigami.Action showAllAction: Kirigami.Action {
        id: showAllAction
        text: i18nc("@action:intoolbar Show all notifications", "All")
        icon.name: "notifications"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked) {
                visibilityMenu.lastCheckedIndex = 0;
                notificationModel.excludeTypes = [];
            }
        }
    }

    property Kirigami.Action mentionOnlyAction: Kirigami.Action {
        id: onlyMentionAction
        text: i18nc("@action:intoolbar Show only mentions", "Mentions")
        icon.name: "tokodon-chat-reply"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked) {
                visibilityMenu.lastCheckedIndex = 1;
                notificationModel.excludeTypes = ['status', 'reblog', 'follow', 'follow_request', 'favourite', 'poll', 'update', 'admin.sign_up', 'admin.report', 'severed_relationships', 'moderation_warning'];
            }
        }
    }

    property Kirigami.Action moreAction: Kirigami.Action {
        icon.name: "view-more-horizontal-symbolic"
        text: i18nc("@action:intoolbar More filter categories", "More")
        checkable: true
        onTriggered: {
            visibilityMenu.tookAction = false;
            visibilityMenu.popup();
        }
    }

    QQC2.Menu {
        id: visibilityMenu

        property int lastCheckedIndex
        property bool tookAction

        visible: false
        onClosed: {
            if (!tookAction) {
                tabBar.currentIndex = lastCheckedIndex;
            }
        }

        QQC2.MenuItem {
            icon.name: "tokodon-post-boost"
            text: i18nc("Show only boosts", "Boosts")
            onTriggered: {
                notificationModel.excludeTypes = ['mention', 'status', 'follow', 'follow_request', 'favourite', 'poll', 'update', 'admin.sign_up', 'admin.report', 'severed_relationships', 'moderation_warning'];
                visibilityMenu.tookAction = true;
            }
        }
        QQC2.MenuItem {
            icon.name: "tokodon-post-favorite"
            text: i18nc("Show only favorites", "Favorites")
            onTriggered: {
                notificationModel.excludeTypes = ['mention', 'status', 'reblog', 'follow', 'follow_request', 'poll', 'update', 'admin.sign_up', 'admin.report', 'severed_relationships', 'moderation_warning'];
                visibilityMenu.tookAction = true;
            }
        }
        QQC2.MenuItem {
            icon.name: "office-chart-bar"
            text: i18nc("Show only poll results", "Poll Results")
            onTriggered: {
                notificationModel.excludeTypes = ['mention', 'status', 'reblog', 'follow', 'follow_request', 'favourite', 'update', 'admin.sign_up', 'admin.report', 'severed_relationships', 'moderation_warning'];
                visibilityMenu.tookAction = true;
            }
        }
        QQC2.MenuItem {
            icon.name: "user-home-symbolic"
            text: i18nc("Show only followed statuses", "Posts")
            onTriggered: {
                notificationModel.excludeTypes = ['mention', 'reblog', 'follow', 'follow_request', 'favourite', 'poll', 'update', 'admin.sign_up', 'admin.report', 'severed_relationships', 'moderation_warning'];
                visibilityMenu.tookAction = true;
            }
        }
        QQC2.MenuItem {
            icon.name: "list-add-user"
            text: i18nc("Show only follows", "Follows")
            onTriggered: {
                notificationModel.excludeTypes = ['mention', 'status', 'reblog', 'follow_request', 'favourite', 'poll', 'update', 'admin.sign_up', 'admin.report', 'severed_relationships', 'moderation_warning'];
                visibilityMenu.tookAction = true;
            }
        }
    }

    header: Kirigami.NavigationTabBar {
        id: tabBar

        anchors.left: parent.left
        anchors.right: parent.right
        actions: [showAllAction, mentionOnlyAction, moreAction]
        enabled: !listview.model.loading

        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
    }

    NotificationModel {
        id: notificationModel
    }

    NotificationGroupingModel {
        id: groupedNotificationModel
        sourceModel: notificationModel
    }

    Component.onCompleted: {
        showAllAction.checked = true

        // TODO: When we can require KF 6.8, set it as a normal property
        if (timelinePage.verticalScrollBarInteractive !== undefined) {
            timelinePage.verticalScrollBarInteractive = false;
        }
    }

    ListView {
        id: listview
        model: timelinePage.currentModel

        Connections {
            target: notificationModel
            function onPostSourceReady(backend, isEdit) {
                const item = pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "StatusComposer"), {
                    purpose: isEdit ? StatusComposer.Edit : StatusComposer.Redraft,
                    backend: backend
                });
                item.refreshData(); // to refresh spoiler text, etc
            }
        }

        delegate: DelegateChooser {
            role: "type"
            DelegateChoice {
                roleValue: Notification.Favorite
                PostDelegate {
                    width: ListView.view.width
                    secondary: true
                    timelineModel: groupedNotificationModel
                    loading: listview.model.loading
                    showSeparator: index !== ListView.view.count - 1
                    showInteractionButton: false
                }
            }

            DelegateChoice {
                roleValue: Notification.Repeat
                PostDelegate {
                    width: ListView.view.width
                    secondary: true
                    timelineModel: groupedNotificationModel
                    loading: listview.model.loading
                    showSeparator: index !== ListView.view.count - 1
                    showInteractionButton: false
                }
            }

            DelegateChoice {
                roleValue: Notification.Mention
                PostDelegate {
                    width: ListView.view.width
                    timelineModel: groupedNotificationModel
                    loading: listview.model.loading
                    showSeparator: index !== ListView.view.count - 1
                }
            }

            DelegateChoice {
                roleValue: Notification.Follow
                FollowDelegate {}
            }

            DelegateChoice {
                roleValue: Notification.Update
                PostDelegate {
                    width: ListView.view.width
                    secondary: true
                    timelineModel: groupedNotificationModel
                    loading: listview.model.loading
                    showSeparator: index !== ListView.view.count - 1
                    showInteractionButton: false
                }
            }

            DelegateChoice {
                roleValue: Notification.Status
                PostDelegate {
                    width: ListView.view.width
                    secondary: true
                    timelineModel: groupedNotificationModel
                    loading: listview.model.loading
                    showSeparator: index !== ListView.view.count - 1
                    showInteractionButton: false
                }
            }

            DelegateChoice {
                roleValue: Notification.Poll
                PostDelegate {
                    width: ListView.view.width
                    secondary: true
                    timelineModel: groupedNotificationModel
                    loading: listview.model.loading
                    showSeparator: index !== ListView.view.count - 1
                    showInteractionButton: false
                }
            }

            DelegateChoice {
                roleValue: Notification.AdminSignUp
                FollowDelegate {}
            }

            DelegateChoice {
                roleValue: Notification.AdminReport
                ReportDelegate {}
            }

            DelegateChoice {
                roleValue: Notification.SeveredRelationships
                RelationshipSeveranceDelegate {}
            }

            DelegateChoice {
                roleValue: Notification.ModerationWarning
                AccountWarningDelegate {}
            }
        }

        Kirigami.LoadingPlaceholder {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "notifications"
            text: i18n("No Notifications")
            explanation: i18n("You have no notifications yet. When people reply, favorite or boost your posts they will show up here.")
            visible: listview.count === 0 && !timelinePage.currentModel.loading
        }
    }
}
