// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import QtGraphicalEffects 1.0

QQC2.AbstractButton {
    id: root

    required property var card

    Layout.fillWidth: true
    Layout.topMargin: visible ? Kirigami.Units.largeSpacing : 0

    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0

    onClicked: Qt.openUrlExternally(root.card.url)

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
        onHoveredChanged: if (hovered) {
            applicationWindow().hoverLinkIndicator.text = root.card.url;
        } else {
            applicationWindow().hoverLinkIndicator.text = "";
        }
    }

    background: Rectangle {
        radius: Kirigami.Units.largeSpacing
        color: 'transparent'
        border {
            width: 1
            color: Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor,0.15)
        }
    }
    contentItem: RowLayout {
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
            Layout.topMargin: 1
            Layout.bottomMargin: 1
            Layout.leftMargin: 1
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
                            corners {
                                bottomLeftRadius: Kirigami.Units.largeSpacing + 1
                                topLeftRadius: Kirigami.Units.largeSpacing + 1
                            }
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
            Kirigami.Heading {
                level: 5
                text: root.card ? root.card.title : ''
                elide: Text.ElideRight
                Layout.fillWidth: true
                wrapMode: root.card && root.card.providerName ? Text.WordWrap : Text.NoWrap
                maximumLineCount: 1
                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                }
            }
            QQC2.Label {
                text: root.card ? root.card.providerName : ''
                elide: Text.ElideRight
                Layout.fillWidth: true
                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }
}
