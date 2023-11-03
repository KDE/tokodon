// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import QtQuick.Templates 2.15 as T
import org.kde.kirigami.delegates as KirigamiDelegates
import org.kde.kitemmodels

import "Components"

Kirigami.OverlaySheet {
    id: controlRoot

    property alias listView: listView

    signal codeSelected(code: string)

    title: i18nc("@title", "Select Language")
    parent: applicationWindow().overlay

    implicitWidth: Kirigami.Units.gridUnit * 30
    implicitHeight: Kirigami.Units.gridUnit * 30

    onOpened: listView.positionViewAtIndex(listView.currentIndex, ListView.Center)

    ListView {
        id: listView

        currentIndex: -1
        model: KSortFilterProxyModel{
            sourceModel: RawLanguageModel {}
            sortRoleName: "preferred"
            sortOrder: Qt.DescendingOrder
        }

        delegate: QQC2.ItemDelegate {
            id: delegate

            required property string name
            required property string code
            required property bool preferred

            width: ListView.view.width
            highlighted: ListView.isCurrentItem
            text: name

            onClicked: {
                controlRoot.codeSelected(code);
                controlRoot.close();
            }

            contentItem: KirigamiDelegates.IconTitleSubtitle {
                title: delegate.text
                icon.source: preferred ? "favorite" : undefined
                selected: delegate.highlighted
            }
        }
    }
}