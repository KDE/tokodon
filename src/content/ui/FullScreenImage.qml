// SPDX-FileCopyrightText: 2019 Black Hat <bhat@encom.eu.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtCore
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.qmlmodels

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.labs.components as Components
import org.kde.tokodon

import "./PostDelegate" as PostDelegate

Components.AlbumMaximizeComponent {
    id: root

    property alias attachments: root.model
    required property var identity
    property string postId

    actions: [
        ShareAction {
            inputData: {
                'urls': [root.currentItem.source],
                'title': "Image",
            }
        },
        Kirigami.Action {
            icon.name: "view-conversation-balloon-symbolic"
            text: i18nc("@action:intoolbar", "Open Post")
            visible: root.postId !== ""
            onTriggered: {
                close();
                Navigation.openPost(root.postId);
            }
        }
    ]

    leading: ClickableIdentityInfo {
        identity: root.identity
        secondary: false
        visible: root.identity
        onClicked: {
            close();
            Navigation.openAccount(root.identity.id);
        }
    }

    onSaveItem: {
        const dialog = saveAsDialog.createObject(applicationWindow().overlay, {
            url: root.currentItem.source,
        })
        dialog.selectedFile = dialog.currentFolder + "/" + FileHelper.fileName(root.currentItem.source);
        dialog.open();
    }

    onItemRightClicked: {
        imageMenu.popup(QQC2.ApplicationWindow.window)
    }

    Component {
        id: saveAsDialog
        FileDialog {
            property var url
            fileMode: FileDialog.SaveFile
            currentFolder: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
            onAccepted: {
                if (!selectedFile) {
                    return;
                }
                console.log(url, selectedFile, AccountManager.selectedAccount)
                FileHelper.downloadFile(AccountManager.selectedAccount, url, selectedFile)
            }
        }
    }

    onClosed: {
        applicationWindow().isShowingFullScreenImage = false;
    }

    onOpened: {
        applicationWindow().isShowingFullScreenImage = true;
        forceActiveFocus();
    }

    AttachmentMenu {
        id: imageMenu
        attachment: root.model[root.currentIndex]
    }
}
