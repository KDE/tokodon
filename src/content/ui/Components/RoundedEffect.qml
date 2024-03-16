// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Effects
import org.kde.kirigami as Kirigami

MultiEffect {
    id: root

    maskEnabled: true
    maskSpreadAtMax: 1
    maskSpreadAtMin: 1
    maskThresholdMin: 0.5
    maskSource: ShaderEffectSource {
        sourceItem: Rectangle {
            width: root.width
            height: root.height
            radius: Kirigami.Units.mediumSpacing
        }
    }
}