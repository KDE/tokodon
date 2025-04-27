// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon


// This is the main text content of a status
QQC2.Label {
    id: root

    required property string content
    required property bool expandedPost
    required property bool secondary
    required property bool shouldOpenInternalLinks
    property bool shouldOpenAnyLinks: true
    property bool hoverEnabled: true

    topPadding: 0
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0
    property string clickedUrl: ""

    Accessible.name: i18nc("@info", "Post content")
    Accessible.description: TextHandler.stripHtml(root.content)

    activeFocusOnTab: true
    text: TextHandler.fixBidirectionality(root.content, Config.defaultFont)
    Layout.fillWidth: true
    textFormat: TextEdit.RichText
    wrapMode: TextEdit.Wrap
    color: root.secondary ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor
    onHoveredLinkChanged: if (hoveredLink.length > 0) {
        applicationWindow().hoverLinkIndicator.text = hoveredLink;
    } else {
        applicationWindow().hoverLinkIndicator.text = "";
    }

    TapHandler {
        acceptedButtons: Qt.RightButton | Qt.LeftButton
        exclusiveSignals: TapHandler.SingleTap | TapHandler.DoubleTap
        // Exclude touchscreen users from this menu until we can figure out a better UX.
        // Currently it's way too easy to trigger this when tapping URLs.
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad | PointerDevice.Stylus

        onSingleTapped: (eventPoint, button) => {
            const point = root.mapFromGlobal(eventPoint.globalPosition.x, eventPoint.globalPosition.y);
            const foundLink = root.linkAt(point.x, point.y);
            if (!foundLink) {
                return;
            }

            if (button === Qt.LeftButton) {
                if (root.shouldOpenAnyLinks) {
                    applicationWindow().navigateLink(foundLink, root.shouldOpenInternalLinks)
                }
                return;
            }

            const linkMenuComponent = Qt.createComponent("org.kde.tokodon", "LinkMenu");
            const linkMenu = linkMenuComponent.createObject(root.QQC2.Overlay.overlay, {
                url: foundLink,
            });

            (linkMenu as LinkMenu)?.popup(root.QQC2.ApplicationWindow.window);
        }
    }

    HoverHandler {
        enabled: root.hoverEnabled
        cursorShape: root.hoveredLink !== '' ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
}
