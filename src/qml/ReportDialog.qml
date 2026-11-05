// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.tokodon

Kirigami.Page {
    id: root

    enum Type {
        Post,
        User
    }

    property int type
    property Identity identity
    property string postId

    title: type === ReportDialog.Post ? KI18n.i18nc("@title", "Report Post") : KI18n.i18nc("@title", "Report User")

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    property ReportEditorBackend backend: ReportEditorBackend {
        accountId: root.identity ? root.identity.id : ""
        postId: root.postId
        comment: reason.text
    }

    Connections {
        target: root.backend

        function onReported() {
            root.Kirigami.PageStack.closeDialog();
        }
    }

    QQC2.TextArea {
        id: reason
        placeholderText: root.type === ReportDialog.Post ? KI18n.i18nc("@title", "Reason for reporting this post") : KI18n.i18nc("@title", "Reason for reporting this user")
        anchors.fill: parent
        wrapMode: TextEdit.Wrap
        enabled: !root.backend.loading
        focus: true

        background: Rectangle {
            color: Kirigami.Theme.backgroundColor
        }
    }

    footer: QQC2.ToolBar {
        enabled: !root.backend.loading

        QQC2.DialogButtonBox {
            anchors.fill: parent
            Item {
                Layout.fillWidth: true
            }
            QQC2.Button {
                text: KI18n.i18nc("@action:button 'Report' as in 'Report this to moderators'", "Report")
                icon.name: "dialog-warning-symbolic"
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.AcceptRole
                onClicked: {
                    root.backend.submit();
                    root.Kirigami.PageStack.closeDialog();
                }
            }
            QQC2.Button {
                icon.name: "dialog-cancel-symbolic"
                text: KI18n.i18nc("@action", "Cancel")
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.RejectRole
                onClicked: root.Kirigami.PageStack.closeDialog()
            }
        }
    }
}
