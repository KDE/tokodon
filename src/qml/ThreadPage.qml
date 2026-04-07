// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.tokodon

TimelinePage {
    id: root

    required property string postId

    expandedPost: true
    showPostAction: false
    showFilterAction: false

    Connections {
        target: root.model

        function onLoadingChanged(): void {
            root.listView.positionViewAtIndex(root.model.getRootIndex(), ListView.Beginning);
        }

        function onHasHiddenRepliesChanged(): void {
            if (root.model.hasHiddenReplies) {
                root.originalPostUrl = root.model.postUrl;
            }
        }
    }

    Connections {
        target: applicationWindow()

        // Refresh the model when we reply to it ourselves
        function onNewPost() {
            model.refresh();
        }
    }

    model: ThreadModel {
        postId: root.postId
    }
}
