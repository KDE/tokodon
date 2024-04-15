// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import QtQuick.Templates 2.15 as T
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.components as Components
import org.kde.kitemmodels

import "Components"

Kirigami.OverlaySheet {
    id: controlRoot

    property alias listView: listView

    signal codeSelected(code: string)

    parent: applicationWindow().overlay

    title: i18nc("@title", "Select Language")

    implicitWidth: Kirigami.Units.gridUnit * 30
    implicitHeight: Kirigami.Units.gridUnit * 30

    padding: 0

    onOpened: listView.positionViewAtIndex(listView.currentIndex, ListView.Center)

    ListView {
        id: listView

        Kirigami.Theme.colorSet: Kirigami.Theme.View

        currentIndex: -1
        model: KSortFilterProxyModel{
            sourceModel: RawLanguageModel {}
            sortRoleName: "preferred"
            sortOrder: Qt.DescendingOrder
        }

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property int index
            required property string name
            required property string code
            required property bool preferred

            highlighted: ListView.isCurrentItem
            text: name
            icon.source: preferred ? "favorite" : undefined

            onClicked: {
                controlRoot.codeSelected(code);
                controlRoot.close();
            }
        }
    }
}
