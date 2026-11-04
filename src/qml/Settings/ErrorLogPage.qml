// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kquickcontrolsaddons as KQuickControlsAddons

import org.kde.tokodon

Kirigami.ScrollablePage {
    id: root

    title: KI18n.i18nc("@title:window", "Error Log")

    actions: QQC2.Action {
        icon.name: "edit-clear-all-symbol"
        text: KI18n.i18nc("@action:intoolbar", "Clear All")
        onTriggered: Controller.clearErrorMessages()
    }

    ListView {
        id: view

        model: Controller.errorMessages
        delegate: ColumnLayout {
            id: delegate

            required property int index
            required property var modelData

            spacing: 0
            width: ListView.view.width

            FormCard.AbstractFormDelegate {
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
                        text: KI18n.i18nc("@action:button", "Copy To Clipboard")
                        icon.name: "edit-copy-symbolic"
                        display: QQC2.Button.IconOnly

                        onClicked: clipboard.content = delegate.modelData.url + ": " + delegate.modelData.message
                    }
                }
            }

            FormCard.FormDelegateSeparator {
                Layout.fillWidth: true
                visible: delegate.index !== view.count - 1
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent

            icon.name: "error"
            text: KI18n.i18nc("@label", "No Errors")
            visible: Controller.errorMessages.length === 0

            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }

    KQuickControlsAddons.Clipboard { id: clipboard }
}
