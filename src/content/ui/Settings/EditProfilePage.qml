// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import QtQuick.Dialogs
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import ".."

Kirigami.Page {
    id: root

    property alias account: editor.account

    readonly property ProfileEditorBackend backend : ProfileEditorBackend {
        account: root.account
        onSendNotification: applicationWindow().showPassiveNotification(message)
    }

    readonly property bool canEditProfile: !AccountManager.accountHasIssue(account)

    title: i18nc("@title:window", "Edit Profile")
    spacing: 0

    ProfileEditor {
        id: editor

        anchors.fill: parent
    }

    footer: editor.profileFooter
}
