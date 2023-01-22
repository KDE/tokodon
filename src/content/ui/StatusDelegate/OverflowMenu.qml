// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kmasto 1.0

QQC2.Menu {
    id: root

    required property int index
    required property string postId
    required property string url
    required property bool bookmarked

    QQC2.MenuItem {
        text: i18n("Expand this post")
        onTriggered: Navigation.openThread(root.id)
    }

    QQC2.MenuItem {
        text: i18n("Open original page")
        onTriggered: {
            Qt.openUrlExternally(root.url)
        }
    }

    QQC2.MenuItem {
        text: i18n("Copy link to this post")
        onTriggered: {
            Clipboard.saveText(root.url)
        }
    }

    QQC2.MenuSeparator {}

    QQC2.MenuItem {
        text: root.bookmarked ? i18n("Remove bookmark") : i18n("Bookmark")
        onTriggered: timelineModel.actionBookmark(timelineModel.index(root.index, 0))
    }
}
