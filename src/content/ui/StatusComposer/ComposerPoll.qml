// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQml.Models
import org.kde.kirigami as Kirigami
import org.kde.tokodon

ColumnLayout {
    id: root

    implicitHeight: Kirigami.Units.gridUnit * 20

    required property var poll
    required property int maxPollOptions
    required property int maxCharactersPerOption

    Repeater {
        model: root.poll.options

        QQC2.ItemDelegate {
            background: null
            visible: addPool.checked
            Layout.fillWidth: true

            required property var modelData
            required property var index

            contentItem: RowLayout {
                spacing: Kirigami.Units.largeSpacing

                QQC2.RadioButton {
                    visible: !poll.multipleChoice
                    enabled: false
                }
                QQC2.CheckBox {
                    visible: poll.multipleChoice
                    enabled: false
                }
                QQC2.TextField {
                    Layout.fillWidth: true
                    placeholderText: i18nc("@label:textbox Poll choice", "Choice %1", index + 1)
                    maximumLength: root.maxCharactersPerOption

                    onEditingFinished: root.poll.setOption(index, text)

                    Component.onCompleted: text = modelData
                }
                QQC2.ToolButton {
                    icon.name: "edit-delete-remove"
                    enabled: poll.options.length > 2
                    onClicked: root.poll.removeOption(index)
                }
            }
        }
    }

    Kirigami.ActionToolBar {
        id: actionsToolbar

        Layout.fillWidth: true

        actions: [
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.KeepVisible

                text: i18nc("@action:intoolbar Poll toolbar", "Add Choice")
                tooltip: i18nc("@info:tooltip Poll toolbar", "Add a new poll choice")
                icon.name: "list-add"
                onTriggered: poll.addOption()
                enabled: poll.options.length < root.maxPollOptions
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.KeepVisible

                displayComponent: QQC2.ComboBox {
                    textRole: "text"
                    valueRole: "time"

                    model: PollTimeModel {
                        id: timeModel
                    }

                    Component.onCompleted: currentIndex = indexOfValue(poll.expiresIn)
                    onActivated: poll.expiresIn = timeModel.getTime(currentIndex)

                    QQC2.ToolTip.text: i18nc("@info:tooltip Poll toolbar", "When the poll will expire")
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    QQC2.ToolTip.visible: hovered
                }
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide

                displayComponent: QQC2.CheckBox {
                    text: i18nc("@option:check Poll toolbar", "Multiple choice")

                    onClicked: poll.multipleChoice = checked

                    Component.onCompleted: checked = poll.multipleChoice

                    QQC2.ToolTip.text: i18nc("@info:tooltip Poll toolbar", "Allow multiple choices")
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    QQC2.ToolTip.visible: hovered
                }
            },
            Kirigami.Action {
                displayHint: Kirigami.DisplayHint.AlwaysHide

                displayComponent: QQC2.CheckBox {
                    text: i18nc("@option:check Poll toolbar", "Hide totals")

                    onClicked: poll.hideTotals = checked

                    Component.onCompleted: checked = poll.hideTotals

                    QQC2.ToolTip.text: i18nc("@info:tooltip Poll toolbar", "Hide vote count until the poll ends")
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    QQC2.ToolTip.visible: hovered
                }
            }
        ]
    }
}

