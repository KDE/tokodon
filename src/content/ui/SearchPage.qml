// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import org.kde.tokodon
import './PostDelegate'

Kirigami.ScrollablePage {
    id: root

    property string pageId
    property alias searchModel: searchView.model
    property string initialSearchTerms

    title: i18nc("@title", "Search")

    header: QQC2.Control {
        topPadding: 0
        bottomPadding: 0
        leftPadding: 0
        rightPadding: 0

        width: root.width

        background: Rectangle {
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            color: Kirigami.Theme.backgroundColor

            Kirigami.Separator {
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                    right: parent.right
                }
            }
        }

        contentItem: ColumnLayout {
            spacing: 0

            Kirigami.SearchField {
                id: searchField

                text: root.initialSearchTerms
                focus: true

                Layout.fillWidth: true
                Layout.margins: Kirigami.Units.largeSpacing

                onAccepted: if (text.length > 2) {
                    root.searchModel.search(text);
                }
            }
        }
    }

    SearchView {
        id: searchView
        text: searchField.text
    }
}
