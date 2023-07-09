// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

QQC2.Menu {
    id: root

    required property string url

    title: i18n("Share")
    enabled: false
}