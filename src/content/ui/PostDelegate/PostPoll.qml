// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts

// Polls inside of a status
ColumnLayout {
    id: root

    required property var index
    required property var poll
    readonly property bool showResults: poll.voted || poll.expired || showVotesSwitch.checked

    QQC2.ButtonGroup {
        id: pollGroup
        exclusive: !root.poll.multiple
    }

    Repeater {
        model: root.showResults ? root.poll.options : []

        ColumnLayout {
            id: votedPollDelegate

            required property int index
            required property var modelData

            RowLayout {
                spacing: root.poll.votesCount !== 0 ? Kirigami.Units.smallSpacing : 0
                QQC2.Label {
                    text: if (votedPollDelegate.modelData.votesCount === -1) {
                        return ''
                    } else if (root.poll.votesCount === 0) {
                        return ''
                    } else {
                        return i18nc("Votes percentage", "%1%", Math.round(votedPollDelegate.modelData.votesCount / root.poll.votesCount * 100))
                    }
                    Layout.alignment: Qt.AlignVCenter
                    Layout.minimumWidth: root.poll.votesCount !== 0 ? Kirigami.Units.gridUnit * 2 : 0
                }

                QQC2.Label {
                    text: votedPollDelegate.modelData.title
                    Layout.alignment: Qt.AlignVCenter
                }

                Kirigami.Icon {
                    source: "checkmark-symbolic"
                    visible: root.poll.hasVotedFor(votedPollDelegate.index)

                    Layout.preferredWidth: Kirigami.Units.iconSizes.sizeForLabels
                    Layout.preferredHeight: Kirigami.Units.iconSizes.sizeForLabels
                    Layout.alignment: Qt.AlignVCenter

                    QQC2.ToolTip.text: i18n("You voted for this option")
                    QQC2.ToolTip.visible: optionHover.hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                    HoverHandler {
                        id: optionHover
                    }
                }
            }

            RowLayout {
                spacing: Kirigami.Units.smallSpacing

                QQC2.ProgressBar {
                    from: 0
                    to: 100
                    value: votedPollDelegate.modelData.votesCount / root.poll.votesCount * 100
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 10
                    Layout.minimumWidth: Kirigami.Units.gridUnit * 10
                    Layout.alignment: Qt.AlignVCenter
                }

                QQC2.Label {
                    text: i18n("(No votes)")
                    visible: root.poll.votesCount === 0
                }
            }
        }
    }

    Repeater {
        model: !root.showResults ? root.poll.options : []
        RowLayout {
            id: resultDelegate

            spacing: Kirigami.Units.smallSpacing

            required property int index
            required property var modelData

            QQC2.CheckBox {
                visible: root.poll.multiple
                Layout.alignment: Qt.AlignVCenter
                QQC2.ButtonGroup.group: pollGroup
                property int choiceIndex: resultDelegate.index
            }

            QQC2.RadioButton {
                visible: !root.poll.multiple
                Layout.alignment: Qt.AlignVCenter
                QQC2.ButtonGroup.group: pollGroup
                property int choiceIndex: resultDelegate.index
            }

            QQC2.Label {
                text: resultDelegate.modelData.title
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }

    RowLayout {
        spacing: Kirigami.Units.smallSpacing
        visible: !root.poll.expired && !root.poll.voted

        QQC2.Button {
            enabled: !root.showResults && pollGroup.checkState !== Qt.Unchecked
            text: i18n("Vote")
            icon.name: "checkbox-symbolic"
            onClicked: {
                let choices = [];
                const buttons = pollGroup.buttons;
                for (let i in buttons) {
                    const button = buttons[i];
                    if (!button.visible) {
                        continue;
                    }

                    if (button.checked) {
                        choices.push(button.choiceIndex);
                    }
                }
                timelineModel.actionVote(timelineModel.index(root.index, 0), choices)
            }
        }

        QQC2.Switch {
            id: showVotesSwitch

            text: i18nc("@option:check Show poll results", "Show Results")
        }
    }

    QQC2.Label {
        visible: root.poll.expired
        text: i18n("Poll has closed")
        color: Kirigami.Theme.disabledTextColor
    }
}
