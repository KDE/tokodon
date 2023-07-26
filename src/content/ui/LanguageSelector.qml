// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0
import QtQuick.Templates 2.15 as T

import "Components"

QQC2.ComboBox {
    id: controlRoot

    model: LanguageModel {}

    textRole: "name"
    valueRole: "code"

    // 1-to-1 copy of qqc2-desktop-style combobox
    popup: T.Popup {
        y: controlRoot.height
        width: controlRoot.width
        implicitHeight: contentItem.implicitHeight
        topMargin: 6
        bottomMargin: 6
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false

        modal: true
        dim: true
        closePolicy: QQC2.Popup.CloseOnEscape | QQC2.Popup.CloseOnPressOutside

        QQC2.Overlay.modal: Item {}

        contentItem: QQC2.ScrollView {
            LayoutMirroring.enabled: controlRoot.mirrored
            LayoutMirroring.childrenInherit: true

            background: Rectangle {
                color: Kirigami.Theme.backgroundColor
            }

            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

            ListView {
                cacheBuffer: 1

                implicitHeight: contentHeight
                model: controlRoot.delegateModel
                delegate: controlRoot.delegate
                currentIndex: controlRoot.highlightedIndex
                boundsBehavior: Flickable.StopAtBounds

                section.property: "preferred"
                section.criteria: ViewSection.FullString
                section.delegate: ColumnLayout {
                    width: ListView.view.width

                    required property string section

                    QQC2.ItemDelegate {
                        Layout.fillWidth: true
                        text: section === "true" ? i18nc("@item:inlistbox Group of preferred languages", "Preferred Languages") :
                                                   i18nc("@item:inlistbox Group of all languages", "All Languages")

                        enabled: false

                        Kirigami.Theme.colorSet: controlRoot.Kirigami.Theme.inherit ? controlRoot.Kirigami.Theme.colorSet : Kirigami.Theme.View
                        Kirigami.Theme.inherit: controlRoot.Kirigami.Theme.inherit
                    }

                    Kirigami.Separator {
                        Layout.fillWidth: true
                    }
                }
            }
        }

        background: PopupShadow {}
    }
}