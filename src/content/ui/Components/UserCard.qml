// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import org.kde.tokodon.private
import '..'
import '../PostDelegate'

Kirigami.AbstractCard {
    id: root

    required property var userIdentity

    property bool isBackgroundAvailable: !userIdentity.backgroundUrl.toString().trim().endsWith("/headers/original/missing.png")

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }

    showClickFeedback: true

    contentItem: Item {
        implicitHeight: userInfo.implicitHeight
        anchors {
            leftMargin: Kirigami.Units.mediumSpacing
            rightMargin: Kirigami.Units.mediumSpacing
            topMargin: Kirigami.Units.mediumSpacing
        }

        ColumnLayout {
            id: userInfo
            InlineIdentityInfo {
                secondary: false
                identity: root.userIdentity
                Layout.bottomMargin: Kirigami.Units.smallSpacing
            }

            QQC2.TextArea {
                id: bio
                visible: root.userIdentity.bio && root.userIdentity.bio.length > 0
                font: Config.defaultFont
                width: root.width - Kirigami.Units.largeSpacing * 2
                text:  "<style>
                            a {
                            color: " + Kirigami.Theme.linkColor + ";
                            text-decoration: none;
                            }
                            </style>" + root.userIdentity.bio
                textFormat: TextEdit.RichText
                readOnly: true
                background: null
                wrapMode: TextEdit.Wrap
                selectByMouse: !Kirigami.Settings.isMobile
                onLinkActivated: {
                    // Tag has a complete url in the form of https://domain.tld/tags/tag
                    const [, , hostname, ...pathSegments] = link.split('/');
                    const path = '/' + pathSegments.join('/');
                    const tag = path.indexOf("/tags/") !== -1 ? path.substring(path.indexOf("/tags/") + 6)
                        : link.startsWith('hashtag:/') ? link.substring(9)
                            : '';
                        tag.length > 0 ? pageStack.push(tagModelComponent, { hashtag: tag }): Qt.openUrlExternally(link);

                }
                color: Kirigami.Theme.textColor
                onHoveredLinkChanged: if (hoveredLink.length > 0) {
                    applicationWindow().hoverLinkIndicator.text = hoveredLink;
                } else {
                    applicationWindow().hoverLinkIndicator.text = "";
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }
    }

    onClicked: {
        Navigation.openAccount(root.userIdentity.id)
    }
}
