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

Components.MessageDialog {
    id: controlRoot

    property alias listView: listView

    signal codeSelected(code: string)

    parent: applicationWindow().overlay

    title: i18nc("@title", "Select Language")

    implicitWidth: Kirigami.Units.gridUnit * 30
    implicitHeight: Kirigami.Units.gridUnit * 30

    padding: 0
    bottomPadding: 1 // HACK: prevent the scrollview to go out of the dialog

    header: ColumnLayout {
        width: parent.width
        spacing: 0

        RowLayout {
            Layout.margins: Kirigami.Units.smallSpacing
            Kirigami.Heading {
                Layout.leftMargin: Kirigami.Units.smallSpacing
                Layout.rightMargin: Kirigami.Units.smallSpacing
                Layout.fillWidth: true
                text: controlRoot.title
            }

            QQC2.ToolButton {
                icon.name: "dialog-close"
                text: i18nc("@action:button", "Close")
                display: QQC2.Button.IconOnly
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }
    }

    footer: null

    onOpened: listView.positionViewAtIndex(listView.currentIndex, ListView.Center)

    contentItem: QQC2.ScrollView {
        clip: true

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
}