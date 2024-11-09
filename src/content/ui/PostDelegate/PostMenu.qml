// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import org.kde.tokodon
import org.kde.kquickcontrolsaddons as KQuickControlsAddons

import ".."

/**
 * @brief The menu located in the "three dots hamburger" icon on a status.
 */
QQC2.Menu {
    id: root

    required property int index
    required property string postId
    required property string url
    required property bool bookmarked
    required property bool isSelf
    required property bool expandedPost
    required property bool pinned
    required property var authorIdentity
    required property bool isPrivate

    readonly property bool hasMultipleAccounts: AccountManager.rowCount() > 1

    signal deletePost
    signal redraftPost

    QQC2.MenuItem {
        icon.name: "expand"
        text: i18nc("@action:inmenu 'Thread' is a series of posts.", "Open Thread")
        onTriggered: Navigation.openPost(root.postId)
        visible: !root.expandedPost
        enabled: visible
    }

    QQC2.MenuItem {
        icon.name: "window"
        text: i18nc("@action:inmenu 'Browser' being a web browser", "Open in Browser")
        onTriggered: {
            Qt.openUrlExternally(root.url)
        }
    }

    QQC2.MenuItem {
        icon.name: "edit-copy"
        text: i18nc("@action:inmenu", "Copy Link")
        onTriggered: {
            clipboard.content = root.url;
            applicationWindow().showPassiveNotification(i18n("Post link copied."));
        }

        KQuickControlsAddons.Clipboard { id: clipboard }
    }

    QQC2.MenuItem {
        icon.name: "view-web-browser-dom-tree"
        text: i18nc("@action Open embed into website dialog", "Embed")
        visible: !root.isPrivate
        enabled: visible

        onTriggered: AccountManager.selectedAccount.fetchOEmbed(root.postId, root.authorIdentity)
    }

    QQC2.MenuSeparator {
        visible: root.hasMultipleAccounts
    }

    QQC2.MenuItem {
        visible: root.hasMultipleAccounts
        enabled: visible
        icon.name: "expand"
        text: i18nc("@action:inmenu", "Open as…")
        onTriggered: applicationWindow().requestCrossAction('open', url)
    }

    QQC2.MenuItem {
        visible: root.hasMultipleAccounts
        enabled: visible
        icon.name: "view-conversation-balloon-symbolic"
        text: i18nc("@action:inmenu", "Reply as…")
        onTriggered: applicationWindow().requestCrossAction('reply', url)
    }

    QQC2.MenuItem {
        visible: root.hasMultipleAccounts
        enabled: visible
        icon.name: "favorite"
        text: i18nc("@action:inmenu", "Favorite as…")
        onTriggered: applicationWindow().requestCrossAction('favourite', url)
    }

    QQC2.MenuItem {
        visible: root.hasMultipleAccounts
        enabled: visible
        icon.name: "boost"
        text: i18nc("@action:inmenu", "Boost as…")
        onTriggered: applicationWindow().requestCrossAction('reblog', url)
    }

    QQC2.MenuItem {
        visible: root.hasMultipleAccounts
        enabled: visible
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
        enabled: visible
        text: root.pinned ? i18nc("@action:inmenu", "Unpin on Profile") : i18nc("@action:inmenu", "Pin on Profile")
        onTriggered: timelineModel.actionPin(timelineModel.index(root.index, 0))
    }

    QQC2.MenuSeparator {
        visible: !root.isSelf
    }

    QQC2.MenuItem {
        icon.name: "dialog-cancel"
        visible: !root.isSelf
        enabled: visible
        text: {
            if (root.authorIdentity.relationship && root.authorIdentity.relationship.muting) {
                return i18nc("@action:inmenu Unmute account", "Unmute @%1", root.authorIdentity.username);
            } else {
                return i18nc("@action:inmenu Mute account", "Mute @%1", root.authorIdentity.username);
            }
        }
        onTriggered: {
            if (root.authorIdentity.relationship && root.authorIdentity.relationship.muting) {
                AccountManager.selectedAccount.unmuteAccount(root.authorIdentity);
            } else {
                AccountManager.selectedAccount.muteAccount(root.authorIdentity);
            }
        }
    }

    QQC2.MenuItem {
        icon.name: "im-ban-kick-user"
        visible: !root.isSelf
        enabled: visible
        text: {
            if (root.authorIdentity.relationship && root.authorIdentity.relationship.blocking) {
                return i18nc("@action:inmenu Unblock account", "Unblock @%1", root.authorIdentity.username);
            } else {
                return i18nc("@action:inmenu Block account", "Block @%1", root.authorIdentity.username);
            }
        }
        onTriggered: {
            if (root.authorIdentity.relationship && root.authorIdentity.relationship.blocking) {
                AccountManager.selectedAccount.unblock(root.authorIdentity.identity);
            } else {
                AccountManager.selectedAccount.block(root.authorIdentity.identity);
            }
        }
    }

    QQC2.MenuItem {
        icon.name: "dialog-warning-symbolic"
        visible: !root.isSelf
        enabled: visible
        text: i18nc("@action:inmenu Report this post", "Report…");
        onTriggered: Navigation.reportPost(root.authorIdentity, root.postId)
    }

    QQC2.MenuSeparator {}

    QQC2.MenuItem {
        icon.name: "edit-entry"
        visible: root.isSelf
        enabled: visible
        text: i18n("Edit")
        onTriggered: timelineModel.actionRedraft(timelineModel.index(root.index, 0), true)
    }

    QQC2.MenuItem {
        icon.name: "edit-delete"
        visible: root.isSelf
        enabled: visible
        text: i18n("Delete")
        onTriggered: root.deletePost()
    }

    QQC2.MenuItem {
        icon.name: "edit-cut"
        visible: root.isSelf
        enabled: visible
        text: i18n("Delete & Re-draft")
        onTriggered: root.redraftPost()
    }

    ShareMenu {
        url: root.url
    }
}
