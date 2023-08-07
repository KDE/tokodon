// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Dialogs 1.3
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
            text: i18n("Show detailed statistics about posts")
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
            text: i18n("Show link preview")
            checked: Config.showLinkPreview
            enabled: !Config.isShowLinkPreviewImmutable
            onToggled: {
                Config.showLinkPreview = checked
                Config.save()
            }
        }

        MobileForm.FormDelegateSeparator { below: showStats; above: cropMedia }

        MobileForm.FormSwitchDelegate {
            id: cropMedia
            text: i18n("Crop images in the timeline to 16:9")
            checked: Config.cropMedia
            onToggled: {
                Config.cropMedia = checked
                Config.save()
            }
        }

        MobileForm.FormDelegateSeparator { below: cropMedia; above: autoPlayGif }

        MobileForm.FormSwitchDelegate {
            id: autoPlayGif
            text: i18n("Auto-play animated GIFs")
            checked: Config.autoPlayGif
            onToggled: {
                Config.autoPlayGif = checked
                Config.save()
            }
        }

        MobileForm.FormDelegateSeparator { below: autoPlayGif; above: colorTheme }

        MobileForm.FormComboBoxDelegate {
            Layout.fillWidth: true
            id: colorTheme
            text: i18n("Color theme")
            textRole: "display"
            valueRole: "display"
            model: ColorSchemer.model
            Component.onCompleted: currentIndex = ColorSchemer.indexForScheme(Config.colorScheme);
            onCurrentValueChanged: {
                ColorSchemer.apply(currentIndex);
                Config.colorScheme = ColorSchemer.nameForIndex(currentIndex);
                Config.save();
            }
        }

        MobileForm.FormDelegateSeparator { below: colorTheme; above: fontSelector }

        MobileForm.FormButtonDelegate {
            id: fontSelector
            text: i18n("Content font")
            description: Config.defaultFont.family + " " + Config.defaultFont.pointSize + "pt"
            onClicked: fontDialog.open()

            FontDialog {
                id: fontDialog
                title: i18n("Please choose a font")
                font: Config.defaultFont
                monospacedFonts: false
                onAccepted: {
                    Config.defaultFont = font;
                    Config.save();
                }
            }
        }
    }
}
