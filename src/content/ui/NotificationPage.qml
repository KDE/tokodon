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
    readonly property bool typesAreGroupable: !mentionOnlyAction.checked && !followsOnlyAction.checked && !pollResultsOnlyAction.checked
    property bool shouldGroupNotifications: typesAreGroupable
    readonly property var currentModel: shouldGroupNotifications ? groupedNotificationModel : notificationModel

    actions: [
        Kirigami.Action {
            icon.name: "checkmark-symbolic"
            text: i18nc("@action:intoolbar Mark all notifications as read", "Mark All As Read")
            enabled: AccountManager.selectedAccount.unreadNotificationsCount > 0
            onTriggered: timelinePage.currentModel.markAllNotificationsAsRead()
        }
    ]

    onBackRequested: if (dialog) {
        dialog.close();
        dialog = null;
        event.accepted = true;
    }

    property Kirigami.Action showAllAction: Kirigami.Action {
        id: showAllAction
        text: i18nc("Show all notifications", "All")
        icon.name: "notifications"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked)
                notificationModel.excludeTypes = [];
        }
    }

    property Kirigami.Action mentionOnlyAction: Kirigami.Action {
        id: onlyMentionAction
        text: i18nc("Show only mentions", "Mentions")
        icon.name: "tokodon-chat-reply"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked)
                notificationModel.excludeTypes = ['status', 'reblog', 'follow', 'follow_request', 'favourite', 'poll', 'update', 'admin.sign_up'];
        }
    }

    property Kirigami.Action boostsOnlyAction: Kirigami.Action {
        text: i18nc("Show only boosts", "Boosts")
        icon.name: "tokodon-post-boost"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked)
                notificationModel.excludeTypes = ['mention', 'status', 'follow', 'follow_request', 'favourite', 'poll', 'update', 'admin.sign_up'];
        }
    }

    property Kirigami.Action favoritesOnlyAction: Kirigami.Action {
        text: i18nc("Show only favorites", "Favorites")
        icon.name: "tokodon-post-favorite"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked)
                notificationModel.excludeTypes = ['mention', 'status', 'reblog', 'follow', 'follow_request', 'poll', 'update', 'admin.sign_up'];
        }
    }

    property Kirigami.Action pollResultsOnlyAction: Kirigami.Action {
        text: i18nc("Show only poll results", "Poll Results")
        icon.name: "office-chart-bar"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked)
                notificationModel.excludeTypes = ['mention', 'status', 'reblog', 'follow', 'follow_request', 'favourite', 'update', 'admin.sign_up'];
        }
    }

    property Kirigami.Action postsOnlyAction: Kirigami.Action {
        text: i18nc("Show only followed statuses", "Posts")
        icon.name: "user-home-symbolic"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked)
                notificationModel.excludeTypes = ['mention', 'reblog', 'follow', 'follow_request', 'favourite', 'poll', 'update', 'admin.sign_up'];
        }
    }

    property Kirigami.Action followsOnlyAction: Kirigami.Action {
        text: i18nc("Show only follows", "Follows")
        icon.name: "list-add-user"
        checkable: true
        onCheckedChanged: (checked) => {
            if (checked)
                notificationModel.excludeTypes = ['mention', 'status', 'reblog', 'follow_request', 'favourite', 'poll', 'update', 'admin.sign_up'];
        }
    }

    header: Kirigami.NavigationTabBar {
        anchors.left: parent.left
        anchors.right: parent.right
        actions: [showAllAction, mentionOnlyAction, favoritesOnlyAction, boostsOnlyAction, pollResultsOnlyAction, postsOnlyAction, followsOnlyAction]
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
                const item = pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
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
                    timelineModel: notificationModel
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
                    timelineModel: notificationModel
                    loading: listview.model.loading
                    showSeparator: index !== ListView.view.count - 1
                    showInteractionButton: false
                }
            }

            DelegateChoice {
                roleValue: Notification.Mention
                PostDelegate {
                    width: ListView.view.width
                    timelineModel: notificationModel
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
                    timelineModel: notificationModel
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
                    timelineModel: notificationModel
                    loading: listview.model.loading
                    showSeparator: index !== ListView.view.count - 1
                    showInteractionButton: false
                }
            }

            DelegateChoice {
                roleValue: Notification.AdminSignUp
                FollowDelegate {}
            }
        }

        QQC2.ProgressBar {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
            indeterminate: true
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No Notifications")
            visible: listview.count === 0 && !timelinePage.currentModel.loading
        }
    }
}
