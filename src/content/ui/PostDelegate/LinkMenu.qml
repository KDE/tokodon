// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-FileCopyrightText: 2024 Arran Ubels <kde@arran4.com>
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
    property string url

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

    QQC2.MenuSeparator {}

    ShareMenu {
        url: root.url
    }
}
