// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import org.kde.tokodon

FormCard.FormCardPage {
    id: root

    title: i18nc("@title:window", "About Server")

    required property string kind

    ServerInformationCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 4

        kind: root.kind
    }
}
