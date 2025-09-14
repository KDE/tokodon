// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

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
