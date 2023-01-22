// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-only

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQml.Models 2.15
import org.kde.kirigami 2.19 as Kirigami

ColumnLayout {
    id: poll
    width: parent.width
    Repeater {
        model: ListModel {
            id: pollModel
            property bool multipleChoice: true
            ListElement {
                name: ""
            }
            ListElement {
                name: ""
            }
        }

        Kirigami.AbstractListItem {
            background: null
            visible: addPool.checked
            contentItem: RowLayout {
                QQC2.RadioButton {
                    autoExclusive: pollModel.multipleChoice
                    QQC2.ToolTip {
                        text: i18n("Make pool auto-exclusive")
                    }
                    TapHandler {
                        onTapped: pollModel.multipleChoice = !pollModel.multipleChoice
                    }
                }
                QQC2.TextField {
                    Layout.fillWidth: true
                    placeholderText: i18n("Choice %1", index + 1)
                }
                QQC2.ToolButton {
                    icon.name: "edit-delete-remove"
                    enabled: pollModel.count > 2
                }
            }
        }
    }
}

