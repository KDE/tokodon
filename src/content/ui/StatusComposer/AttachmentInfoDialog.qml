// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import Qt5Compat.GraphicalEffects

import "../Components"

QQC2.Dialog {
    id: root

    property alias text: textArea.text
    property alias preview: image.source
    property real focusX: 0.0
    property real focusY: 0.0

    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    implicitHeight: Kirigami.Units.gridUnit * 30
    implicitWidth: Kirigami.Units.gridUnit * 40

    padding: Kirigami.Units.largeSpacing
    bottomPadding: 0

    modal: true

    standardButtons: QQC2.Dialog.Apply | QQC2.Dialog.Cancel

    onApplied: root.close()
    onRejected: root.close()

    contentItem: RowLayout {
        id: layout

        property real focusX: -((focusTarget.x / imageContainer.width) * 2 - 1)
        property real focusY: -((focusTarget.y / imageContainer.height) * 2 - 1)
        property bool ready: false

        // For some reason, the bindings do not work and I need to set these manually, ugh.
        onFocusXChanged: {
            if (ready) {
                root.focusX = focusX;
            }
        }
        onFocusYChanged: {
            if (ready) {
                root.focusY = focusY;
            }
        }

        function handleResize() {
            layout.ready = false;
            focusTarget.x = ((-root.focusX + 1) / 2) * imageContainer.width;
            focusTarget.y = ((-root.focusY + 1) / 2) * imageContainer.height;
            layout.ready = true;
        }

        // Sigh, the dialog relayouts so we can't depend on the height when Component.onCompleted is called.
        // To work around this, run a short timer.
        Timer {
            id: resizeTimer

            interval: 100
            running: true
            repeat: false
            onTriggered: layout.handleResize();
        }

        // Re-uses the timer when resizing to prevent it from being called too often.
        onWidthChanged: {
            if (ready) {
                ready = false;
                resizeTimer.restart();
            }
        }

        onHeightChanged: {
            if (ready) {
                ready = false;
                resizeTimer.restart();
            }
        }

        ColumnLayout {
            Layout.preferredWidth: Kirigami.Units.gridUnit * 15
            Layout.fillHeight: true

            Kirigami.Heading {
                text: i18nc("@info:label Alternate text or description of the imaage", "Description")
                level: 2
                Layout.fillWidth: true
            }

            QQC2.TextArea {
                id: textArea
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        ColumnLayout {
            Layout.preferredWidth: Kirigami.Units.gridUnit * 25
            Layout.fillHeight: true

            Kirigami.Heading {
                text: i18n("Focal point")
                level: 2
                Layout.fillWidth: true
            }

            Rectangle {
                id: imageContainer

                color: "black"

                Layout.fillWidth: true
                Layout.fillHeight: true

                clip: true

                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: Item {
                        width: imageContainer.width
                        height: imageContainer.height
                        Rectangle {
                            anchors.centerIn: parent
                            width: imageContainer.width
                            height: imageContainer.height
                            radius: Kirigami.Units.mediumSpacing
                        }
                    }
                }

                Image {
                    id: image

                    anchors.fill: parent

                    fillMode: Image.PreserveAspectFit

                    FocusedImage {
                        id: preview

                        z: 1
                        opacity: previewHover.hovered ? 0.5 : 1

                        Behavior on opacity {
                            NumberAnimation {
                            }
                        }

                        focusX: layout.focusX
                        focusY: layout.focusY

                        source: image.source

                        anchors {
                            right: parent.right
                            rightMargin: Kirigami.Units.mediumSpacing

                            bottom: parent.bottom
                            bottomMargin: Kirigami.Units.mediumSpacing
                        }

                        width: parent.width / 2
                        height: width * (9.0 / 16.0)

                        layer.enabled: true
                        layer.effect: OpacityMask {
                            maskSource: Item {
                                width: preview.width
                                height: preview.height
                                Rectangle {
                                    anchors.centerIn: parent
                                    width: preview.width
                                    height: preview.height
                                    radius: Kirigami.Units.mediumSpacing
                                }
                            }
                        }

                        HoverHandler {
                            id: previewHover
                        }
                    }
                }

                Rectangle {
                    id: focusTarget

                    width: Kirigami.Units.gridUnit * 3
                    height: Kirigami.Units.gridUnit * 3

                    color: "transparent"
                    opacity: layout.ready ? 1 : 0

                    Behavior on opacity {
                        NumberAnimation {
                        }
                    }

                    border {
                        width: 5
                        color: "white"
                    }

                    radius: width

                    transform: Translate {
                        x: -focusTarget.width / 2
                        y: -focusTarget.height / 2
                    }

                    MouseArea {
                        anchors.fill: parent

                        cursorShape: Qt.PointingHandCursor

                        drag {
                            target: focusTarget
                            axis: Drag.XAndYAxis

                            minimumX: 0
                            maximumX: imageContainer.width

                            minimumY: 0
                            maximumY: imageContainer.height
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: footer.padding = Kirigami.Units.largeSpacing;
}
