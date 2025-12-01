// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import org.kde.tokodon
import './PostDelegate'

Kirigami.ScrollablePage {
    id: root

    property string pageId
    property alias searchModel: searchView.model

    title: i18nc("@title", "Search")

    header: QQC2.Control {
        contentItem: Kirigami.SearchField {
            id: searchField

            onAccepted: if (text.length > 2) {
                root.searchModel.search(text)
            }
        }
    }

    SearchView {
        id: searchView
        text: searchField.text
    }
}
