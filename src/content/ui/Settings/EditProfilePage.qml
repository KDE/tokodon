// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.tokodon
import QtQuick.Dialogs
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as KirigamiComponents
import ".."

Kirigami.Page {
    id: root

    required property AbstractAccount account

    readonly property bool canEditProfile: !AccountManager.accountHasIssue(account)

    title: i18nc("@title:window", "Edit Profile")
    spacing: 0

    ProfileEditor {
        id: editor

        account: root.account

        anchors.fill: parent
    }

    footer: editor.profileFooter
}
