// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon

Kirigami.Page {
    id: root

    enum Type {
        Post,
        User
    }

    property var type
    property var identity
    property string postId

    title: type === ReportDialog.Post ? i18nc("@title", "Report Post") : i18nc("@title", "Report User")

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    property ReportEditorBackend backend: ReportEditorBackend {
        accountId: root.identity !== undefined ? root.identity.id : ""
        postId: root.postId
        comment: reason.text
    }

    Connections {
        target: backend

        function onReported() {
            root.closeDialog();
        }
    }

    QQC2.TextArea {
        id: reason
        placeholderText: type === ReportDialog.Post ? i18nc("@title", "Reason for reporting this post") : i18nc("@title", "Reason for reporting this user")
        anchors.fill: parent
        wrapMode: TextEdit.Wrap
        enabled: !backend.loading
        focus: true

        background: Rectangle {
            color: Kirigami.Theme.backgroundColor
        }
    }

    footer: QQC2.ToolBar {
        enabled: !backend.loading

        QQC2.DialogButtonBox {
            anchors.fill: parent
            Item {
                Layout.fillWidth: true
            }
            QQC2.Button {
                text: i18nc("@action:button 'Report' as in 'Report this to moderators'", "Report")
                icon.name: "dialog-warning-symbolic"
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.AcceptRole
                onClicked: {
                    root.backend.submit();
                    root.closeDialog();
                }
            }
            QQC2.Button {
                icon.name: "dialog-cancel-symbolic"
                text: i18nc("@action", "Cancel")
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.RejectRole
                onClicked: root.closeDialog()
            }
        }
    }
}
