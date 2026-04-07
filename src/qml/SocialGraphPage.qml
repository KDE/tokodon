// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@proton.me>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.kirigamiaddons.delegates as Delegates

import "./PostDelegate"

Kirigami.ScrollablePage {
    id: root

    property alias model: listview.model

    title: model.displayName
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

    ListView {
        id: listview

        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property var index
            required property var identity

            text: identity.displayName

            onClicked: Navigation.openAccount(delegate.identity.id)

            contentItem: ColumnLayout {
                spacing: 0

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Layout.fillWidth: true

                    InlineIdentityInfo {
                        identity: delegate.identity
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    QQC2.Button {
                        text: i18nc("@action:button Allow follow request", "Allow")
                        icon.name: "checkmark"
                        onClicked: root.model.actionAllow(root.model.index(delegate.index, 0))
                        visible: root.model.isFollowRequest
                    }

                    QQC2.Button {
                        text: i18nc("@action:button Deny follow request", "Deny")
                        icon.name: "cards-block"
                        onClicked: root.model.actionDeny(root.model.index(delegate.index, 0))
                        visible: root.model.isFollowRequest
                    }

                    QQC2.Button {
                        text: i18nc("@action:button Deny follow request", "Unfollow")
                        icon.name: "list-remove-user"
                        onClicked: root.model.actionUnfollow(root.model.index(delegate.index, 0))
                        visible: root.model.isFollowing && root.model.accountId === AccountManager.selectedAccount.identity.id
                    }

                    QQC2.Button {
                        text: i18nc("@action:button Deny follow request", "Remove Follower")
                        icon.name: "list-remove-user"
                        onClicked: root.model.actionRemoveFollower(root.model.index(delegate.index, 0))
                        visible: root.model.isFollower && root.model.accountId === AccountManager.selectedAccount.identity.id
                    }

                    QQC2.Button {
                        text: i18nc("@action:button Deny follow request", "Remove")
                        icon.name: "list-remove-user"
                        onClicked: root.model.actionRemoveFromList(root.model.index(delegate.index, 0))
                        visible: root.model.isList && root.model.accountId === AccountManager.selectedAccount.identity.id
                    }

                    QQC2.Button {
                        text: i18nc("@action:button Unblock this user", "Unblock")
                        onClicked: root.model.actionUnblock(root.model.index(delegate.index, 0))
                        visible: root.model.isBlockList
                    }

                    QQC2.Button {
                        text: i18nc("@action:button Unmute this user", "Unmute")
                        onClicked: root.model.actionUnmute(root.model.index(delegate.index, 0))
                        visible: root.model.isMuteList
                    }
                }

                QQC2.ProgressBar {
                    visible: listview.model.loading && (delegate.index == listview.count - 1)
                    indeterminate: true
                    padding: Kirigami.Units.largeSpacing * 2

                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                }
            }
        }

        Kirigami.LoadingPlaceholder {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: listview.model.placeholderIconName
            text: listview.model.placeholderText
            explanation: listview.model.placeholderExplanation
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
