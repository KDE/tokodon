// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

QQC2.TextArea {
    id: root

    required property string content
    required property bool expandedPost
    required property bool secondary
    required property bool shouldOpenInternalLinks

    signal clicked()

    activeFocusOnTab: false

    font: Config.defaultFont
    Layout.fillWidth: true
    text: "<style>
        a {
        color: " + Kirigami.Theme.linkColor + ";
        text-decoration: none;
        }
        </style>" + root.content
    textFormat: TextEdit.RichText
    topPadding: 0
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0
    readOnly: true
    background: null
    wrapMode: Text.Wrap
    selectByMouse: !Kirigami.Settings.isMobile && root.expandedPost
    onLinkActivated: (link) => applicationWindow().navigateLink(link, root.shouldOpenInternalLinks)
    color: root.secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
    onHoveredLinkChanged: if (hoveredLink.length > 0) {
        applicationWindow().hoverLinkIndicator.text = hoveredLink;
    } else {
        applicationWindow().hoverLinkIndicator.text = "";
    }

    TapHandler {
        enabled: !parent.hoveredLink && !root.expandedPost
        onTapped: root.clicked()
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton // don't eat clicks on the Text
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}