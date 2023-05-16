// SPDX-FileCopyrightText: 2023 Nate Graham <nate@kde.org>
// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only


import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2

import org.kde.kirigami 2.19 as Kirigami

QQC2.Button {
    id: root

    required property string title
    required property string subtitle

    leftPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing

    Accessible.name: title + " " + subtitle

    contentItem: ColumnLayout {
        spacing: 0

        // Title
        Kirigami.Heading {
            Layout.fillWidth: true
            level: 4
            text: root.title
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            maximumLineCount: 2
            elide: Text.ElideRight
            wrapMode: Text.Wrap
        }

        // Subtitle
        QQC2.Label {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: root.subtitle
            text: root.subtitle
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            opacity: 0.6
            verticalAlignment: Text.AlignTop
        }
    }
}
