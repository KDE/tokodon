// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.ki18n
import org.kde.kirigami as Kirigami

/**
 * Android (currently no-op version) of ShareAction.
 */
Kirigami.Action {
    id: shareAction
    icon.name: "emblem-shared-symbolic"
    text: KI18n.i18n("Share")
    tooltip: KI18n.i18n("Share the selected media")

    visible: false

    property var inputData: ({})
}
