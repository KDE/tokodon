// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQml
import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts

import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigami as Kirigami

import org.kde.tokodon
import org.kde.tokodon.private

FormCard.FormCardPage {
    FormCard.FormHeader {
        title: i18nc("@title:group", "General")
    }

    FormCard.FormCard {
        FormCard.FormComboBoxDelegate {
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

        FormCard.FormDelegateSeparator {
            below: popoutComposer; above: colorTheme
            visible: !Kirigami.Settings.isMobile
        }

        FormCard.FormSwitchDelegate {
            id: popoutComposer
            text: i18n("Pop out the post composer by default")
            checked: Config.popOutByDefault
            enabled: !Config.popOutByDefaultImmutable
            visible: !Kirigami.Settings.isMobile
            onToggled: {
                Config.popOutByDefault = checked
                Config.save()
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Posts")
    }

    FormCard.FormCard {
        FormCard.FormSwitchDelegate {
            id: showStats
            text: i18n("Show number of replies, favorites and boosts")
            checked: Config.showPostStats
            enabled: !Config.isShowPostStatsImmutable
            onToggled: {
                Config.showPostStats = checked
                Config.save()
            }
        }

        FormCard.FormDelegateSeparator {
            below: showStats; above: showLinkPreview
        }

        FormCard.FormSwitchDelegate {
            id: showLinkPreview
            text: i18n("Show link previews")
            checked: Config.showLinkPreview
            enabled: !Config.isShowLinkPreviewImmutable
            onToggled: {
                Config.showLinkPreview = checked
                Config.save()
            }
        }

        FormCard.FormDelegateSeparator {
            below: showLinkPreview; above: fontSelector
        }

        FormCard.FormButtonDelegate {
            id: fontSelector
            text: i18n("Content font")
            description: Config.defaultFont.family + " " + Config.defaultFont.pointSize + "pt"
            onClicked: fontDialog.open()

            FontDialog {
                id: fontDialog
                title: i18n("Please choose a font")
                selectedFont: Config.defaultFont
                onAccepted: {
                    Config.defaultFont = selectedFont;
                    Config.save();
                }
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Media")
    }

    FormCard.FormCard {
        FormCard.FormSwitchDelegate {
            id: cropMedia
            text: i18n("Crop images on the timeline")
            description: i18n("If unchecked, posts with only one image attached will be uncropped and shown in full.")
            checked: Config.cropMedia
            onToggled: {
                Config.cropMedia = checked
                Config.save()
            }
        }

        FormCard.FormDelegateSeparator {
            below: cropMedia; above: autoPlayGif
        }

        FormCard.FormSwitchDelegate {
            id: autoPlayGif
            text: i18n("Auto-play animated GIFs")
            checked: Config.autoPlayGif
            onToggled: {
                Config.autoPlayGif = checked
                Config.save()
            }
        }
    }
}
