// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later


import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import Qt.labs.qmlmodels
import org.kde.kirigami as Kirigami
import org.kde.tokodon as Tokodon

QQC2.ItemDelegate {
    id: root

    required property int index

    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    leftPadding: Kirigami.Units.largeSpacing * 2
    rightPadding: Kirigami.Units.largeSpacing * 2

    highlighted: false
    hoverEnabled: false

    width: ListView.view.width

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor

        Kirigami.Separator {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }

            width: flexColumn.innerWidth
        }
    }

    contentItem: Kirigami.FlexColumn {
        id: flexColumn

        maximumWidth: Kirigami.Units.gridUnit * 40
        spacing: 0
        padding: 0

        QQC2.Label {
            text: i18nc("@info", "Unknown notification")
            wrapMode: Text.WordWrap

            Layout.fillWidth: true
        }
    }
}
