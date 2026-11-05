// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.tokodon

import org.kde.kirigamiaddons.delegates as Delegates
import "./PostDelegate"
import "./StatusComposer"

Kirigami.Page {
    id: root
    title: KI18n.i18nc("@title:window Public feeds of this or other servers", "Public Feeds")

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    property Kirigami.Action thisServerAction: Kirigami.Action {
        icon.name: "system-users"
        text: KI18n.i18nc("@action:button Filter for public posts from the user's current server", "This Server")
        checkable: true
    }

    property Kirigami.Action otherServersAction: Kirigami.Action {
        icon.name: "kstars_xplanet"
        text: KI18n.i18nc("@action:button Filter for public posts from other servers other than the current user's", "Other Servers")
        checkable: true
    }

    header: Kirigami.NavigationTabBar {
        anchors.left: parent.left
        anchors.right: parent.right
        actions: [root.thisServerAction, root.otherServersAction]

        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
    }

    Component.onCompleted: thisServerAction.checked = true

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    StackLayout {
        anchors.fill: parent

        currentIndex: {
            if (root.otherServersAction.checked) {
                return 1;
            }
            return 0;
        }

        QQC2.ScrollView {
            clip: true

            Keys.onPressed: event => localTimelineView.handleKeyEvent(event)

            QQC2.ScrollBar.vertical.interactive: false

            TimelineView {
                id: localTimelineView

                model: MainTimelineModel {
                    id: localModel
                    name: "public"
                    shouldLoadMore: root.thisServerAction.checked
                }

                Kirigami.PlaceholderMessage {
                    icon.name: "view-conversation-balloon-symbolic"
                    text: KI18n.i18nc("@info:placeholder", "No Posts")
                    visible: !localModel.loading && localTimelineView.count === 0
                    anchors.centerIn: parent
                    width: parent.width - Kirigami.Units.gridUnit * 4
                }
            }
        }

        QQC2.ScrollView {
            clip: true

            Keys.onPressed: event => globalTimelineView.handleKeyEvent(event)

            QQC2.ScrollBar.vertical.interactive: false

            TimelineView {
                id: globalTimelineView

                model: MainTimelineModel {
                    id: globalModel
                    name: "federated"
                    shouldLoadMore: root.otherServersAction.checked
                }

                Kirigami.PlaceholderMessage {
                    icon.name: "view-conversation-balloon-symbolic"
                    text: KI18n.i18nc("@info:placeholder", "No Posts")
                    visible: !globalModel.loading && globalTimelineView.count === 0
                    anchors.centerIn: parent
                    width: parent.width - Kirigami.Units.gridUnit * 4
                }
            }
        }
    }
}
