// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls 2 as QQC2
import org.kde.kirigami 2 as Kirigami

QQC2.Menu {
    id: root

    required property string url

    title: i18n("Share")
    enabled: false
}