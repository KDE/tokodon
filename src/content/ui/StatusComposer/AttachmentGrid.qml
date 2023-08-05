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

    visible: attachmentsRepeater.count > 0
    columns: Math.min(attachmentsRepeater.count, 2)
    implicitHeight: Kirigami.Units.gridUnit * 20

    Repeater {
        id: attachmentsRepeater

        model: root.attachmentEditorModel

        Image {
            id: img

            required property int index
            required property string preview
            required property string description

            readonly property var mediaRatio: 9.0 / 16.0

            // If there is three attachments, the first one is bigger than the other two.
            readonly property bool isSpecialAttachment: index === 0 && attachmentsRepeater.count === 3

            readonly property var heightDivisor: (isSpecialAttachment || attachmentsRepeater.count < 3) ? 1 : 2

            fillMode: Image.PreserveAspectCrop
            source: img.preview

            Layout.rowSpan: isSpecialAttachment ? 2 : 1

            readonly property real extraSpacing: isSpecialAttachment ? root.rowSpacing : 0

            Layout.preferredWidth: parent.width / root.columns
            Layout.preferredHeight: (parent.width * mediaRatio / heightDivisor) + extraSpacing

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
                        text: img.caption,
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
