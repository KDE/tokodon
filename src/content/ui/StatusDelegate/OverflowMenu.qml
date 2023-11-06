// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import org.kde.tokodon

import ".."

// The menu located in the "three dots hamburger" icon on a status.
QQC2.Menu {
    id: root

    required property int index
    required property string postId
    required property string url
    required property bool bookmarked
    required property bool isSelf
    required property bool expandedPost
    required property bool pinned

    readonly property bool hasMultipleAccounts: AccountManager.rowCount() > 1

    signal deletePost
    signal redraftPost

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

    QQC2.MenuSeparator {
        visible: root.hasMultipleAccounts
    }

    QQC2.MenuItem {
        visible: root.hasMultipleAccounts
        icon.name: "expand"
        text: i18nc("@action:inmenu", "Open as…")
        onTriggered: applicationWindow().requestCrossAction('open', url)
    }

    QQC2.MenuItem {
        visible: root.hasMultipleAccounts
        icon.name: "tokodon-post-reply"
        text: i18nc("@action:inmenu", "Reply as…")
        onTriggered: applicationWindow().requestCrossAction('reply', url)
    }

    QQC2.MenuItem {
        visible: root.hasMultipleAccounts
        icon.name: "tokodon-post-favorite"
        text: i18nc("@action:inmenu", "Favorite as…")
        onTriggered: applicationWindow().requestCrossAction('favourite', url)
    }

    QQC2.MenuItem {
        visible: root.hasMultipleAccounts
        icon.name: "tokodon-post-boost"
        text: i18nc("@action:inmenu", "Boost as…")
        onTriggered: applicationWindow().requestCrossAction('reblog', url)
    }

    QQC2.MenuItem {
        visible: root.hasMultipleAccounts
        icon.name: "bookmark-new"
        text: i18nc("@action:inmenu", "Bookmark as…")
        onTriggered: applicationWindow().requestCrossAction('bookmark', url)
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
        onTriggered: root.deletePost()
    }

    QQC2.MenuItem {
        icon.name: "edit-cut"
        visible: root.isSelf
        text: i18n("Delete & Re-draft")
        onTriggered: root.redraftPost()
    }

    ShareMenu {
        url: root.url
    }
}
