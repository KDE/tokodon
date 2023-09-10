// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import Qt5Compat.GraphicalEffects

QQC2.AbstractButton {
    id: root

    required property var card

    Accessible.name: i18n("Link preview: %1", root.card ? root.card.title : '')
    Accessible.description: root.card ? root.card.providerName : ''

    leftPadding: Kirigami.Units.largeSpacing
    topPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing

    onClicked: Qt.openUrlExternally(root.card.url)

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
        onHoveredChanged: if (hovered) {
            applicationWindow().hoverLinkIndicator.text = root.card.url;
        } else {
            applicationWindow().hoverLinkIndicator.text = "";
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    background: Rectangle {
        radius: Kirigami.Units.largeSpacing
        color: Kirigami.Theme.backgroundColor
        border {
            width: root.visualFocus ? 2 : 0
            color: root.visualFocus ? Kirigami.Theme.focusColor : 'transparent'
        }
    }

    contentItem: RowLayout {
        spacing: 0

        Rectangle {
            id: logo
            visible: root.card && root.card.image
            color: Kirigami.Theme.backgroundColor
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            radius: Kirigami.Units.largeSpacing
            Layout.minimumHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumHeight: Kirigami.Units.gridUnit * 3
            Layout.minimumWidth: Kirigami.Units.gridUnit * 3
            Layout.maximumWidth: Kirigami.Units.gridUnit * 3
            Layout.topMargin: 0
            Layout.bottomMargin: 0
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Image {
                id: img
                mipmap: true
                smooth: true
                sourceSize {
                    width: logo.width
                    height: logo.height
                }

                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: Item {
                        width: img.width
                        height: img.height
                        Kirigami.ShadowedRectangle {
                            anchors.centerIn: parent
                            radius: Kirigami.Units.largeSpacing
                            width: img.width
                            height: img.height
                        }
                    }
                }

                fillMode: Image.PreserveAspectCrop
                anchors {
                    fill: parent
                }
                source: root.card ? root.card.image : ''
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing
            Layout.leftMargin: Kirigami.Units.largeSpacing
            spacing: 0
            QQC2.Label {
                text: root.card ? root.card.providerName : ''
                elide: Text.ElideRight
                font: Kirigami.Theme.smallFont
                visible: text
                Layout.fillWidth: true
                maximumLineCount: 1
            }
            Kirigami.Heading {
                level: 5
                text: root.card ? root.card.title : ''
                elide: Text.ElideRight
                font.weight: Font.DemiBold
                maximumLineCount: 1
                visible: text
                Layout.fillWidth: true
            }
            QQC2.Label {
                text: root.card ? root.card.description : ''
                elide: Text.ElideRight
                visible: text
                maximumLineCount: 1
                Layout.fillWidth: true
            }
        }
    }

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }
}
