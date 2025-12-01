// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Window
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.tokodon

Kirigami.ScrollablePage {
    title: i18n("Conversations")

    actions: [
        Kirigami.Action {
            icon.name: "view-conversation-balloon-symbolic"
            text: i18nc("@action:intoolbar", "New Conversation…")
            onTriggered: pageStack.layers.push(addUserComponent)
        }
    ]

    Component {
        id: addUserComponent

        Kirigami.ScrollablePage {
            id: addUserPage

            title: i18nc("@title", "Search Users")

            ListView {
                id: listView

                model: SearchModel {
                    id: searchModel
                }

                delegate: Delegates.RoundedItemDelegate {
                    id: delegate

                    required property var index
                    required property var authorIdentity

                    text: authorIdentity.displayName

                    onClicked: {
                        delegate.Window.window.pageStack.layers.pop();
                        Navigation.openConversation(authorIdentity.account);
                    }

                    contentItem: ColumnLayout {
                        spacing: 0

                        InlineIdentityInfo {
                            identity: delegate.authorIdentity

                            Layout.fillWidth: true
                        }

                        QQC2.ProgressBar {
                            visible: delegate.ListView.view.model.loading && (index == listview.count - 1)
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

                header: QQC2.Control {
                    topPadding: 0
                    bottomPadding: 0
                    leftPadding: 0
                    rightPadding: 0

                    width: listView.width

                    background: Rectangle {
                        Kirigami.Theme.colorSet: Kirigami.Theme.Window
                        Kirigami.Theme.inherit: false
                        color: Kirigami.Theme.backgroundColor

                        Kirigami.Separator {
                            anchors {
                                left: parent.left
                                bottom: parent.bottom
                                right: parent.right
                            }
                        }
                    }

                    contentItem: ColumnLayout {
                        spacing: 0

                        Kirigami.SearchField {
                            id: searchField
                            placeholderText: i18n("Search for user…")

                            focus: true

                            Layout.fillWidth: true
                            Layout.margins: Kirigami.Units.largeSpacing

                            onAccepted: if (text.length > 2) {
                                listView.model.search(text, "accounts", false)
                            }
                        }
                    }
                }
            }
        }
    }

    ListView {
        id: conversationView

        currentIndex: -1
        model: ConversationModel {
            id: conversationModel
        }
        delegate: ConversationDelegate {
            conversationsCount: conversationView.count
            onMarkAsRead: (conversationId) => {
                conversationModel.markAsRead(conversationId)
            }
        }

        Kirigami.LoadingPlaceholder {
            visible: conversationView.count === 0 && conversationView.model.loading
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "view-conversation-balloon-symbolic"
            text: i18n("No Conversations")
            explanation: i18n("Direct messages to other users will show up here. Do not share any sensitive information over a conversation.")
            visible: conversationView.count === 0 && !conversationView.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
