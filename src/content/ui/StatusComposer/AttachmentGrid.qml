// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQml.Models 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.0

GridLayout {
    id: root

    required property var attachmentEditorModel
    readonly property var mediaRatio: 9.0 / 16.0

    visible: attachmentsRepeater.count > 0
    columns: attachmentsRepeater.count === 0 > 1 ? 2 : 1
    implicitHeight: Kirigami.Units.gridUnit * 20

    Repeater {
        id: attachmentsRepeater

        model: root.attachmentEditorModel

        Image {
            id: img

            required property int index
            required property string preview
            required property string description

            readonly property var widthDivisor: attachmentsRepeater.count > 1 ? 2 : 1

            // the first attachment in a three attachment set is displayed at full height
            readonly property var heightDivisor: img.isSpecialAttachment ? 1 : (attachmentsRepeater.count > 2 ? 2 : 1)
            Layout.rowSpan: img.isSpecialAttachment ? 2 : 1

            fillMode: Image.PreserveAspectCrop
            source: img.preview

            Layout.preferredWidth: parent.width / widthDivisor
            Layout.preferredHeight: (root.width * root.mediaRatio) / heightDivisor + (img.isSpecialAttachment ? attachmentGridLayout.rowSpacing : 0)

            mipmap: true
            cache: true

            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: Item {
                    width: img.width
                    height: img.height
                    Rectangle {
                        anchors.centerIn: parent
                        width: img.width
                        height: img.height
                        radius: Kirigami.Units.smallSpacing
                    }
                }
            }

            QQC2.RoundButton {
                id: editButton

                QQC2.ToolTip.text: i18n("Edit")
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                QQC2.ToolTip.visible: hovered

                icon.name: 'document-edit'

                onClicked: {
                    const dialog = attachmentInfoDialog.createObject(applicationWindow(), {
                        text: img.description,
                    });
                    dialog.open();
                    dialog.applied.connect(() => {
                        root.attachmentEditorModel.setDescription(img.index, dialog.text);

                    });
                }

                anchors {
                    right: removeButton.left
                    top: parent.top
                    margins: Kirigami.Units.smallSpacing
                    rightMargin: Kirigami.Units.largeSpacing
                }

                Component {
                    id: attachmentInfoDialog

                    AttachmentInfoDialog {}
                }
            }

            QQC2.RoundButton {
                id: removeButton

                QQC2.ToolTip.text: i18n("Remove")
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                QQC2.ToolTip.visible: hovered

                icon.name: 'edit-delete-remove'
                onClicked: backend.attachmentEditorModel.removeAttachment(img.index)

                anchors {
                    right: parent.right
                    top: parent.top
                    margins: Kirigami.Units.smallSpacing
                }
            }
        }
    }
}
