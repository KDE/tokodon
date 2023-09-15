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

        QQC2.ProgressBar {
            visible: conversationView.count === 0 && conversationView.model.loading
            anchors.centerIn: parent
            indeterminate: true
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No Conversations")
            visible: conversationView.count === 0 && !conversationView.model.loading
        }
    }
}
