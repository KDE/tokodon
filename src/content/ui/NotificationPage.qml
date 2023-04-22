// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0
import "./StatusDelegate"
import "./StatusComposer"

Kirigami.ScrollablePage {
    id: timelinePage
    title: i18n("Notifications")

    required property var model
    property var dialog: null

    property alias listViewHeader: listview.header

    actions.main: Kirigami.Action {
        icon.name: "list-add"
        text: i18n("Toot")
        enabled: AccountManager.hasAccounts
        onTriggered: Navigation.openStatusComposer()
    }

    property Kirigami.Action showAllAction: Kirigami.Action {
        id: showAllAction
        text: i18nc("Show all notifications", "All")
        icon.name: "notifications"
        checkable: true
        checked: true
        onCheckedChanged: if (checked) {
            notificationModel.excludeTypes = []
        }
    }

    property Kirigami.Action mentionOnlyAction: Kirigami.Action {
        id: onlyMentionAction
        text: i18nc("Show only mentions", "Mentions")
        icon.name: "tokodon-chat-reply"
        checkable: true
        onCheckedChanged: if (checked) {
            notificationModel.excludeTypes = ['status', 'reblog', 'follow', 'follow', 'follow_request', 'favourite', 'poll', 'update']
        }
    }

    property Kirigami.Action favoritesOnlyAction: Kirigami.Action {
        text: i18nc("Show only favorites", "Favorites")
        icon.name: "like-post"
        checkable: true
        onCheckedChanged: if (checked) {
            notificationModel.excludeTypes = ['mention', 'status', 'reblog', 'follow', 'follow', 'follow_request', 'poll', 'update']
        }
    }

    property Kirigami.Action boostsOnlyAction: Kirigami.Action {
        text: i18nc("Show only boosts", "Boosts")
        icon.name: "boost-post"
        checkable: true
        onCheckedChanged: if (checked) {
            notificationModel.excludeTypes = ['mention', 'status', 'follow', 'follow', 'follow_request', 'favourite', 'poll', 'update']
        }
    }

    property Kirigami.Action pollResultsOnlyAction: Kirigami.Action {
        text: i18nc("Show only poll results", "Poll results")
        icon.name: "folder-chart"
        checkable: true
        onCheckedChanged: if (checked) {
            notificationModel.excludeTypes = ['mention', 'status', 'reblog', 'follow', 'follow', 'follow_request', 'favourite', 'update']
        }
    }

    property Kirigami.Action followsOnlyAction: Kirigami.Action {
        text: i18nc("Show only follows", "Follows")
        icon.name: "list-add-user"
        checkable: true
        onCheckedChanged: if (checked) {
            notificationModel.excludeTypes = ['mention', 'status', 'reblog', 'follow_request', 'favourite', 'poll', 'update']
        }
    }

    header: Kirigami.NavigationTabBar {
        anchors.left: parent.left
        anchors.right: parent.right
        actions: [showAllAction, mentionOnlyAction, favoritesOnlyAction, boostsOnlyAction, pollResultsOnlyAction, followsOnlyAction]
    }

    ListView {
        id: listview
        model: NotificationModel {
            id: notificationModel
        }

        Connections {
            target: notificationModel
            function onPostSourceReady(backend) {
                pageStack.layers.push("./StatusComposer/StatusComposer.qml", {
                    purpose: StatusComposer.Edit,
                    backend: backend
                });
            }
        }

        Component {
            id: fullScreenImage
            FullScreenImage {}
        }
        delegate: DelegateChooser {
            role: "type"
            DelegateChoice {
                roleValue: Notification.Favorite
                StatusDelegate {
                    secondary: true
                    timelineModel: notificationModel
                }
            }

            DelegateChoice {
                roleValue: Notification.Repeat
                StatusDelegate {
                    secondary: true
                    timelineModel: notificationModel
                }
            }

            DelegateChoice {
                roleValue: Notification.Mention
                StatusDelegate {
                    timelineModel: notificationModel
                }
            }

            DelegateChoice {
                roleValue: Notification.Follow
                FollowDelegate {}
            }
            DelegateChoice {
                roleValue: Notification.Update
                StatusDelegate {
                    secondary: true
                    timelineModel: notificationModel
                }
            }

            DelegateChoice {
                roleValue: Notification.Poll
                StatusDelegate {
                    secondary: true
                    timelineModel: notificationModel
                }
            }
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("Loading...")
            visible: listview.count === 0 && listview.model.loading
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No Notifications")
            visible: listview.count === 0 && !listview.model.loading
        }
    }
}
