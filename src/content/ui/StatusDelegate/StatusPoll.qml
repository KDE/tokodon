// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon

// Polls inside of a status
ColumnLayout {
    id: root

    required property var index
    required property var poll
    readonly property bool showResults: poll.voted || poll.expired

    QQC2.ButtonGroup {
        id: pollGroup
        exclusive: !root.poll.multiple
    }

    Repeater {
        model: root.showResults ? root.poll.options : []

        ColumnLayout {
            id: votedPollDelegate

            required property var modelData

            RowLayout {
                spacing: root.poll.votesCount !== 0 ? Kirigami.Units.largeSpacing : 0
                QQC2.Label {
                    text: if (modelData.votesCount === -1) {
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
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
                }
            }

            RowLayout {
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
        model: !root.showResults ? poll.options : []
        RowLayout {
            QQC2.CheckBox {
                visible: poll.multiple
                Layout.alignment: Qt.AlignVCenter
                QQC2.ButtonGroup.group: pollGroup
                property int choiceIndex: index
            }

            QQC2.RadioButton {
                visible: !poll.multiple
                Layout.alignment: Qt.AlignVCenter
                QQC2.ButtonGroup.group: pollGroup
                property int choiceIndex: index
            }

            QQC2.Label {
                text: modelData.title
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }

    QQC2.Button {
        visible: !root.showResults
        text: i18n("Vote")
        enabled: pollGroup.checkState !== Qt.Unchecked
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
}
