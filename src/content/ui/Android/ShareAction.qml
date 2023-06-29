// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

/**
 * Android (currently no-op version) of ShareAction.
 */
Kirigami.Action {
    id: shareAction
    icon.name: "emblem-shared-symbolic"
    text: i18n("Share")
    tooltip: i18n("Share the selected media")

    visible: false

    property var inputData: ({})
}