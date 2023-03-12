// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.20 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import './StatusDelegate'

Kirigami.ScrollablePage {
    id: root

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