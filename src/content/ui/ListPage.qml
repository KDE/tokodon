// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.tokodon

TimelinePage {
    id: root

    property alias listId: timelineModel.listId
    required property string name

    title: name

    showPostAction: false

    model: MainTimelineModel {
        id: timelineModel
        name: "list"
    }
}
