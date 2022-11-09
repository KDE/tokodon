// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami
import org.kde.kmasto 1.0
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

MobileForm.FormCard {
    Layout.topMargin: Kirigami.Units.largeSpacing
    Layout.fillWidth: true
    contentItem: ColumnLayout {
        spacing: 0
        MobileForm.FormCardHeader {
            title: i18n("General")
        }

        MobileForm.FormSwitchDelegate {
            id: showStats
            text: i18n("Show detailed statistics about posts.")
            checked: Config.showPostStats
            enabled: !Config.isShowPostStatsImmutable
            onToggled: {
                Config.showPostStats = checked
                Config.save()
            }
        }

        MobileForm.FormDelegateSeparator { below: showStats; above: showLinkPreview }

        MobileForm.FormSwitchDelegate {
            id: showLinkPreview
            text: i18n("Show link preview.")
            checked: Config.showLinkPreview
            enabled: !Config.isShowLinkPreviewImmutable
            onToggled: {
                Config.showLinkPreview = checked
                Config.save()
            }
        }
    }
}
