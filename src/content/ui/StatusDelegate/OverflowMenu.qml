// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import org.kde.tokodon

import ".."

QQC2.Menu {
    id: root

    required property int index
    required property string postId
    required property string url
    required property bool bookmarked
    required property bool isSelf
    required property bool expandedPost
    required property bool pinned

    QQC2.MenuItem {
        icon.name: "expand"
        text: i18nc("@action:inmenu", "Expand Post")
        onTriggered: Navigation.openThread(root.postId)
        visible: !root.expandedPost
    }

    QQC2.MenuItem {
        icon.name: "window"
        text: i18n("Open Original Page")
        onTriggered: {
            Qt.openUrlExternally(root.url)
        }
    }

    QQC2.MenuItem {
        icon.name: "edit-copy"
        text: i18nc("@action:inmenu", "Copy Link")
        onTriggered: {
            Clipboard.saveText(root.url)
            applicationWindow().showPassiveNotification(i18n("Post link copied."));
        }
    }

    QQC2.MenuSeparator {}

    QQC2.MenuItem {
        icon.name: "bookmark-new"
        text: root.bookmarked ? i18nc("@action:inmenu", "Remove Bookmark") : i18nc("@action:inmenu", "Bookmark")
        onTriggered: timelineModel.actionBookmark(timelineModel.index(root.index, 0))
    }

    QQC2.MenuItem {
        icon.name: root.pinned ? "window-unpin" : "pin"
        visible: root.isSelf
        text: root.pinned ? i18nc("@action:inmenu", "Unpin on Profile") : i18nc("@action:inmenu", "Pin on Profile")
        onTriggered: timelineModel.actionPin(timelineModel.index(root.index, 0))
    }

    QQC2.MenuSeparator {
        visible: root.isSelf
    }

    QQC2.MenuItem {
        icon.name: "edit-entry"
        visible: root.isSelf
        text: i18n("Edit")
        onTriggered: timelineModel.actionRedraft(timelineModel.index(root.index, 0), true)
    }

    QQC2.MenuItem {
        icon.name: "edit-delete"
        visible: root.isSelf
        text: i18n("Delete")
        onTriggered: timelineModel.actionDelete(timelineModel.index(root.index, 0))
    }

    QQC2.MenuItem {
        icon.name: "edit-cut"
        visible: root.isSelf
        text: i18n("Delete & Re-draft")
        onTriggered: {
            timelineModel.actionRedraft(timelineModel.index(root.index, 0), false)
            timelineModel.actionDelete(timelineModel.index(root.index, 0))
        }
    }

    ShareMenu {
        url: root.url
    }
}
