// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigamiaddons.components as Components

import org.kde.tokodon

Components.MessageDialog {
    id: root

    implicitWidth: Math.min(parent.width - Kirigami.Units.gridUnit * 2, Kirigami.Units.gridUnit * 25)
    title: i18nc("@title", "Boost Alt Text Reminder")
    dialogType: Components.MessageDialog.Warning

    QQC2.Label {
        text: i18nc("@label", "Some media in this post is missing alt text, which hinders those who are visually impaired.")
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }

    standardButtons: QQC2.Dialog.Cancel | QQC2.Dialog.Ok
    dontShowAgainName: 'missingBoostAltText'

    Component.onCompleted: {
        const button = standardButton(QQC2.Dialog.Ok);
        button.icon.name = "boost";
        button.text = i18nc("@action:button Boost/Reblog this post anyway", "Boost");
    }
}
