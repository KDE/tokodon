// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import org.kde.tokodon

import ".."

QQC2.Menu {
    id: root

    modal: true

    QQC2.MenuItem {
        icon.name: "window"
        text: i18n("Save Image as....")
    }

    QQC2.MenuItem {
        icon.name: "edit-copy"
        text: i18n("Copy Image...")
    }

    ShareMenu {
        url: "test"
    }
}
