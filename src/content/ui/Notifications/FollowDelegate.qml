// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import Qt.labs.qmlmodels
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.statefulapp as StatefulApp
import org.kde.tokodon

import "../PostDelegate"
import "../Components"

QQC2.ItemDelegate {
    id: root

    required property int index
    required property var notificationActorIdentity
    required property bool selected
    required property string relativeTime

    required property var type
    readonly property bool isAdminSignUp: type === Notification.AdminSignUp
    readonly property bool isFollowRequest: type === Notification.FollowRequest

    padding: 0
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    leftPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing

    topInset: 0
    leftInset: 0
    rightInset: 0
    bottomInset: 0

    highlighted: false
    hoverEnabled: false

    width: ListView.view.width

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor

        Kirigami.Separator {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }

            width: flexColumn.innerWidth
        }
    }

    contentItem: Kirigami.FlexColumn {
        id: flexColumn

        maximumWidth: Kirigami.Units.gridUnit * 40
        spacing: Kirigami.Units.largeSpacing
        padding: 0

        RowLayout {
            spacing: Kirigami.Units.smallSpacing
            Layout.topMargin: visible ? Kirigami.Units.smallSpacing : 0
            Layout.bottomMargin: visible ? Kirigami.Units.smallSpacing : 0
            Kirigami.Icon {
                source: 'list-add-user'
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                color: Kirigami.Theme.disabledTextColor
                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
            }

            QQC2.Label {
                font: Config.defaultFont
                text: if (root.isAdminSignUp) {
                    i18n("%1 signed up", root.notificationActorIdentity.displayNameHtml)
                } else if(root.isFollowRequest) {
                    i18n("%1 requested to follow you", root.notificationActorIdentity.displayNameHtml)
                } else {
                    i18n("%1 followed you", root.notificationActorIdentity.displayNameHtml)
                }
                textFormat: Text.StyledText
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
            }

            QQC2.Label {
                text: root.relativeTime
                color: Kirigami.Theme.disabledTextColor
            }
        }

        UserCard {
            userIdentity: root.notificationActorIdentity

            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
        }

        QQC2.Button {
            text: i18nc("@action:button", "Manage Follow Requests")
            icon.name: "list-add-user"
            visible: root.isFollowRequest

            onClicked: (root.QQC2.ApplicationWindow.window as StatefulApp.StatefulWindow)?.application.action("follow_requests").trigger()

            Layout.fillWidth: true
        }
    }
}
