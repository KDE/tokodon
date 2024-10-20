// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon

Kirigami.ScrollablePage {
    title: i18n("Conversations")

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
            icon.name: "tokodon-chat-reply"
            text: i18n("No Conversations")
            explanation: i18n("Direct messages to other users will show up here. Do not share any sensitive information over a conversation.")
            visible: conversationView.count === 0 && !conversationView.model.loading
        }
    }
}
