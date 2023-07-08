// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kmasto 1.0

TimelinePage {
    id: root

    required property string postId

    expandedPost: true
    showPostAction: false

    model: ThreadModel {
        postId: root.postId
    }
}

