// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import org.kde.tokodon.private
import '..'
import '../PostDelegate'

ListView {
    id: root

    delegate: PostDelegate {
        id: status

        timelineModel: root.model
        expandedPost: false
        showSeparator: index !== ListView.view.count - 1
        loading: root.model.loading
        width: ListView.view.width
    }
}