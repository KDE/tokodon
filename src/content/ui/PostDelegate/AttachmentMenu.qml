// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtCore
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami
import org.kde.tokodon
import org.kde.kirigamiaddons.components as Components

import ".."

/**
 * @brief The context menu for image attachments.
 */
Components.ConvergentContextMenu {
    id: root

    property var attachment: null

    readonly property Component saveAsDialog: FileDialog {
        required property var url
        fileMode: FileDialog.SaveFile
        currentFolder: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
        onAccepted: {
            if (!currentFile) {
                return;
            }
            FileHelper.downloadFile(AccountManager.selectedAccount, url, currentFile);
        }
    }

    QQC2.Action {
        enabled: root.attachment !== null

        icon.name: "window"
        text: {
            if (root.attachment === null) {
                return '';
            }

            if (root.attachment.attachmentType === Attachment.Image) {
                return i18n("Save Image As…");
            } else if (root.attachment.attachmentType === Attachment.GifV) {
                return i18n("Save Gif As…");
            } else if (root.attachment.attachmentType === Attachment.Video) {
                return i18n("Save Video As…");
            } else if (root.attachment.attachmentType === Attachment.Audio) {
                return i18n("Save Audio As…");
            }
            return ''
        }

        onTriggered: {
            const dialog = saveAsDialog.createObject(applicationWindow().overlay, {
                url: root.attachment.source,
            });
            dialog.selectedFile = dialog.currentFolder + "/" + FileHelper.fileName(root.attachment.source);
            dialog.open();
        }
    }

    Kirigami.Action {
        enabled: root.attachment !== null
        visible: root.attachment?.attachmentType === Attachment.Image

        icon.name: "edit-copy"
        text: i18n("Copy Image")
        onTriggered: root.attachment.copyToClipboard()
    }

    Kirigami.Action {
        separator: true
    }

    ShareAction {
        enabled: root.attachment !== null

        inputData: ({
            urls: [root.attachment?.source ?? ''],
            title: i18nc("@title", "Post"),
            mimeType: '*',
        })
    }
}
