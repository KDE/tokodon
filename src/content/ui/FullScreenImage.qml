// SPDX-FileCopyrightText: 2019 Black Hat <bhat@encom.eu.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import Qt.labs.qmlmodels 1.0

import org.kde.kirigami 2.15 as Kirigami
import org.kde.kirigamiaddons.labs.components 1.0 as Components
import org.kde.kmasto 1.0

import "./StatusDelegate" as StatusDelegate

Components.AlbumMaximizeComponent {
    id: root

    property alias attachments: root.model
    required property var identity

    actions: ShareAction {
        inputData: {
            'urls': [content.currentItem.source],
            'mimeType': ["text/uri-list"]
        }
    }

    leading: StatusDelegate.InlineIdentityInfo {
        identity: root.identity
        secondary: false
    }

    onSaveItem: {
        const dialog = saveAsDialog.createObject(QQC2.ApplicationWindow.overlay, {
            url: content.currentItem.source,
        })
        dialog.open();
        dialog.currentFile = dialog.folder + "/" + FileHelper.fileName(content.currentItem.source);
    }

    Component {
        id: saveAsDialog
        FileDialog {
            property var url
            fileMode: FileDialog.SaveFile
            folder: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
            onAccepted: {
                if (!currentFile) {
                    return;
                }
                console.log(url, currentFile, AccountManager.selectedAccount)
                FileHelper.downloadFile(AccountManager.selectedAccount, url, currentFile)
            }
        }
    }

    onClosed: {
        applicationWindow().isShowingFullScreenImage = false;
    }

    onOpened: {
        applicationWindow().isShowingFullScreenImage = true;
    }
}
