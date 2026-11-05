// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.tokodon
import ".."

Kirigami.Page {
    id: root

    required property AbstractAccount account

    readonly property bool canEditProfile: !AccountManager.accountHasIssue(account)

    title: KI18n.i18nc("@title:window", "Edit Profile")
    spacing: 0

    ProfileEditor {
        id: editor

        account: root.account

        anchors.fill: parent
    }

    footer: editor.profileFooter
}
