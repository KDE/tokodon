// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami
import org.kde.kmasto 1.0

Kirigami.ScrollablePage {
    title: i18n("General")
    ColumnLayout {
        Kirigami.FormLayout {
            QQC2.CheckBox {
                text: i18n("Show detailed statistics about posts.")
                checked: Config.showPostStats
                enabled: !Config.isShowPostStatsImmutable
                onToggled: {
                    Config.showPostStats = checked
                    Config.save()
                }
            }
        }
    }
}
