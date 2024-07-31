// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtCore
import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Dialogs
import org.kde.tokodon

import ".."

/**
 * @brief The context menu for image attachments.
 */
QQC2.Menu {
    id: root

    property var attachment

    modal: true

    Component {
        id: saveAsDialog
        FileDialog {
            property var url
            fileMode: FileDialog.SaveFile
            currentFolder: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
            onAccepted: {
                if (!currentFile) {
                    return;
                }
                FileHelper.downloadFile(AccountManager.selectedAccount, url, currentFile);
            }
        }
    }

    QQC2.MenuItem {
        enabled: root.attachment !== null

        icon.name: "window"
        text: {
            if (root.attachment === null) {
                return;
            }

            if (root.attachment.attachmentType === Attachment.Image) {
                return i18n("Save Image As…");
            } else if (root.attachment.attachmentType === Attachment.GifV) {
                return i18n("Save Gif As…");
            } else if (root.attachment.attachmentType === Attachment.Video) {
                return i18n("Save Video As…");
            }
        }

        onTriggered: {
            const dialog = saveAsDialog.createObject(applicationWindow().overlay, {
                url: root.attachment.source,
            });
            dialog.selectedFile = dialog.currentFolder + "/" + FileHelper.fileName(root.attachment.source);
            dialog.open();
        }
    }

    QQC2.MenuItem {
        enabled: root.attachment !== null
        visible: root.attachment !== null && root.attachment.attachmentType === Attachment.Image

        icon.name: "edit-copy"
        text: i18n("Copy Image")
        onTriggered: root.attachment.copyToClipboard()
    }

    QQC2.MenuSeparator {}

    ShareMenu {
        enabled: root.attachment !== null

        url: root.attachment !== null && root.attachment !== undefined ? root.attachment.source : ""
    }
}
