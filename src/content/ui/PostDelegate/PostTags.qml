// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2

import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon

/**
 * @brief The line of tags that are considered "standalone" (e.g. are not embedded in a paragraph)
 *
 * This usually sits below the media attachment
 */
QQC2.Control {
    id: root

    required property var standaloneTags

    readonly property bool hasScrollbar: scrollView.QQC2.ScrollBar.horizontal.size !== 1.0
    readonly property bool scrollAtEnd: (scrollView.QQC2.ScrollBar.horizontal.position + scrollView.QQC2.ScrollBar.horizontal.size) === 1.0

    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0
    topPadding: 0

    visible: root.standaloneTags.length !== 0

    background: null

    contentItem: Item {
        implicitWidth: scrollView.implicitWidth
        implicitHeight: scrollView.implicitHeight

        QQC2.ScrollView {
            id: scrollView

            anchors.fill: parent

            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

            RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Repeater {
                    model: root.standaloneTags

                    Kirigami.Chip {
                        required property string modelData

                        closable: false
                        checkable: false

                        text: "#" + modelData

                        onClicked: Navigation.openTag(modelData)
                    }
                }
            }
        }

        Rectangle {
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }

            gradient: Gradient {
                orientation: Gradient.Horizontal

                GradientStop {
                    position: 0.9; color: Kirigami.Theme.backgroundColor
                }
                GradientStop {
                    position: 1.0; color: "transparent"
                }
            }

            visible: root.hasScrollbar && scrollView.QQC2.ScrollBar.horizontal.position > 0.0
            implicitWidth: prevButton.implicitWidth

            QQC2.ToolButton {
                id: prevButton

                height: parent.height

                icon.name: "go-previous"
                autoRepeat: true
                onClicked: scrollView.QQC2.ScrollBar.horizontal.decrease()
            }
        }

        Rectangle {
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }

            gradient: Gradient {
                orientation: Gradient.Horizontal

                GradientStop {
                    position: 0.0; color: "transparent"
                }
                GradientStop {
                    position: 0.1; color: Kirigami.Theme.backgroundColor
                }
            }

            visible: root.hasScrollbar && !root.scrollAtEnd
            implicitWidth: nextButton.implicitWidth

            QQC2.ToolButton {
                id: nextButton

                height: parent.height

                icon.name: "go-next"
                autoRepeat: true
                onClicked: scrollView.QQC2.ScrollBar.horizontal.increase()
            }
        }
    }
}
