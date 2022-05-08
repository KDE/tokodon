// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.14 as Kirigami
import org.kde.kmasto 1.0

Kirigami.ScrollablePage {
    id: timelinePage
    title: i18n("Notifications")

    required property var model

    property alias listViewHeader: listview.header
    property bool isProfile: false

    actions.main: Kirigami.Action {
        icon.name: "list-add"
        text: i18n("Toot")
        enabled: AccountManager.hasAccounts
        onTriggered: {
            const post = AccountManager.selectedAccount.newPost()
            pageStack.layers.push("qrc:/content/ui/TootComposer.qml", {
                postObject: post
            });
        }
    }

    ListView {
        id: listview
        model: NotificationModel {}

        Component {
            id: fullScreenImage
            FullScreenImage {}
        }
        delegate: DelegateChooser {
            role: "type"
            DelegateChoice {
                roleValue: Notification.Favorite
                PostDelegate {
                    secondary: true
                }
            }

            DelegateChoice {
                roleValue: Notification.Repeat
                PostDelegate {
                    secondary: true
                }
            }

            DelegateChoice {
                roleValue: Notification.Mention
                PostDelegate {
                }
            }

            DelegateChoice {
                roleValue: Notification.Follow
                Kirigami.BasicListItem {
                    text: i18n("%1 followed you", model.actorDisplayName)
                }
            }
        }
    }
}
