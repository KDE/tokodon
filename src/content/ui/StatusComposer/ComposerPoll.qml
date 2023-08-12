// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0

ColumnLayout {
    id: root

    implicitHeight: Kirigami.Units.gridUnit * 20

    required property var poll
    required property int maxPollOptions

    Repeater {
        model: root.poll.options

        Kirigami.AbstractListItem {
            background: null
            visible: addPool.checked

            required property var modelData
            required property var index

            contentItem: RowLayout {
                QQC2.RadioButton {
                    autoExclusive: !poll.multipleChoice
                    enabled: false
                }
                QQC2.TextField {
                    Layout.fillWidth: true
                    placeholderText: i18nc("@label:textbox Poll choice", "Choice %1", index + 1)

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

