// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kquickcontrolsaddons as KQuickControlsAddons

import org.kde.tokodon

Kirigami.Page {
    id: root

    title: i18nc("@title:window", "Error Log")

    actions: [
        Kirigami.Action {
            icon.name: "edit-clear-all"
            text: i18nc("@action:intoolbar", "Clear All")
            onTriggered: Controller.clearErrorMessages()
        }
    ]

    topPadding: 0
    leftPadding: 0
    rightPadding: 0

    FormCard.FormCard {
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            topMargin: Kirigami.Units.largeSpacing * 4
        }

        visible: Controller.errorMessages.length !== 0

        Repeater {
            model: Controller.errorMessages

            FormCard.AbstractFormDelegate {
                id: delegate

                required property var modelData

                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    ColumnLayout {
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Label {
                            text: delegate.modelData.url
                            color: Kirigami.Theme.disabledTextColor
                            elide: Text.ElideRight

                            Layout.fillWidth: true
                        }

                        QQC2.Label {
                            text: delegate.modelData.message
                            wrapMode: Text.WordWrap

                            Layout.fillWidth: true
                        }
                    }

                    QQC2.ToolButton {
                        text: i18nc("@action:button", "Copy To Clipboard")
                        icon.name: "edit-copy-symbolic"
                        display: QQC2.Button.IconOnly

                        onClicked: clipboard.content = delegate.modelData.url + ": " + delegate.modelData.message
                    }
                }
            }
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent

        icon.name: "error"
        text: i18nc("@label", "No Errors")
        visible: Controller.errorMessages.length === 0

        width: parent.width - Kirigami.Units.gridUnit * 4
    }

    KQuickControlsAddons.Clipboard { id: clipboard }
}
