// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import org.kde.kirigami 2.20 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

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
    }
}