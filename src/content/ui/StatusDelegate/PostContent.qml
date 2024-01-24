// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.tokodon.private

// This is the main text content of a status
QQC2.TextArea {
    id: root

    required property string content
    required property bool expandedPost
    required property bool secondary
    required property bool shouldOpenInternalLinks

    signal clicked()

    topPadding: 0
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0

    // Work around QTBUG 93281
    Component.onCompleted: if (text.includes("<img")) {
        FileHelper.forceRefreshTextDocument(root.textDocument, root);
    }

    font: Config.defaultFont
    Layout.fillWidth: true
    text: TextPreprocessing.preprocessHTML(root.content, Kirigami.Theme.linkColor)
    textFormat: TextEdit.RichText
    activeFocusOnTab: false
    readOnly: true
    background: null
    wrapMode: TextEdit.Wrap
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
        cursorShape: root.hoveredLink !== '' ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
