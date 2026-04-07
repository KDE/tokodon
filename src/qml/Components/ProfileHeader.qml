// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiComponents
import org.kde.tokodon

import '..'

QQC2.Pane {
    id: pane

    required property string backgroundUrl
    required property string avatarUrl
    required property string displayName
    required property string account

    background: Item {
        Item {
            anchors.fill: parent

            Rectangle {
                anchors.fill: parent
                color: avatar.color
                opacity: 0.2
            }
            Kirigami.Icon {
                visible: source
                scale: 1.8
                anchors.fill: parent

                source: pane.backgroundUrl

                implicitWidth: 512
                implicitHeight: 512
            }

            layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
            layer.effect: MultiEffect {
                saturation: 1.9

                layer {
                    enabled: true
                    effect: MultiEffect {
                        blurEnabled: true
                        autoPaddingEnabled: false
                        blurMax: 100
                        blur: 1.0
                    }
                }
            }
        }

        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: -1.0; color: "transparent" }
                GradientStop { position: 1.0; color: Kirigami.Theme.backgroundColor }
            }
        }
    }

    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    contentItem: RowLayout {
        spacing: Kirigami.Units.largeSpacing

        implicitHeight: Kirigami.Units.gridUnit * 5

        Layout.maximumWidth: Kirigami.Units.gridUnit * 30
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter

        Kirigami.ShadowedRectangle {
            Layout.margins: Kirigami.Units.gridUnit * 2
            Layout.preferredWidth: Kirigami.Units.gridUnit * 5
            Layout.preferredHeight: Kirigami.Units.gridUnit * 5

            color: Kirigami.Theme.backgroundColor
            radius: width

            shadow {
                size: 15
                xOffset: 5
                yOffset: 5
                color: Qt.rgba(0, 0, 0, 0.2)
            }

            KirigamiComponents.Avatar {
                id: avatar

                height: parent.height
                width: height

                name: pane.displayName
                source: pane.avatarUrl
                imageMode: KirigamiComponents.Avatar.ImageMode.AdaptiveImageOrInitals
            }
        }

        ColumnLayout {
            spacing: 0

            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            QQC2.Label {
                Layout.fillWidth: true
                text: pane.displayName
                font.bold: true
                font.pixelSize: 24
                maximumLineCount: 2
                wrapMode: Text.Wrap
                elide: Text.ElideRight
            }

            QQC2.TextArea {
                text: '@' + pane.account
                textFormat: TextEdit.PlainText
                wrapMode: TextEdit.Wrap
                readOnly: true
                background: null
                font.pixelSize: 18

                leftPadding: 0
                rightPadding: 0
                topPadding: 0

                Layout.fillWidth: true
            }
        }
    }
}
