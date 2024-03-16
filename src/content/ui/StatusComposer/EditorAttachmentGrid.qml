// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQml.Models
import QtQuick.Layouts

import "../Components"

GridLayout {
    id: root

    required property var attachmentEditorModel

    visible: attachmentsRepeater.count > 0
    columns: Math.min(attachmentsRepeater.count, 2)
    implicitHeight: Kirigami.Units.gridUnit * 20

    Repeater {
        id: attachmentsRepeater

        model: root.attachmentEditorModel

        FocusedImage {
            id: img

            required property int index
            required property string preview
            required property string description
            required property real focalX
            required property real focalY

            readonly property var mediaRatio: 9.0 / 16.0

            // If there is three attachments, the first one is bigger than the other two.
            readonly property bool isSpecialAttachment: index === 0 && attachmentsRepeater.count === 3

            readonly property var heightDivisor: (isSpecialAttachment || attachmentsRepeater.count < 3) ? 1 : 2

            source: img.preview
            focusX: img.focalX
            focusY: img.focalY

            Layout.rowSpan: isSpecialAttachment ? 2 : 1

            readonly property real extraSpacing: isSpecialAttachment ? root.rowSpacing : 0

            Layout.preferredWidth: parent.width / root.columns
            Layout.preferredHeight: (parent.width * mediaRatio / heightDivisor) + extraSpacing

            layer.enabled: true
            layer.effect: RoundedEffect {}

            QQC2.RoundButton {
                id: editButton

                QQC2.ToolTip.text: i18n("Edit")
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                QQC2.ToolTip.visible: hovered

                icon.name: 'document-edit'

                onClicked: {
                    const dialog = attachmentInfoDialog.createObject(applicationWindow(), {
                        text: img.description,
                        preview: img.preview,
                        focusX: img.focalX,
                        focusY: img.focalY,
                    });
                    dialog.open();
                    dialog.applied.connect(() => {
                        root.attachmentEditorModel.setDescription(img.index, dialog.text);
                        root.attachmentEditorModel.setFocusPoint(img.index, dialog.focusX, dialog.focusY);
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
