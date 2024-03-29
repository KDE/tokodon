// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQml
import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegates
import org.kde.sonnet 1 as Sonnet
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.delegates 1 as Delegates

FormCard.FormCardPage {
    FormCard.FormCard {
        id: card

        Layout.topMargin: Kirigami.Units.largeSpacing

        readonly
        property
        Sonnet.Settings
        settings: Sonnet.Settings
        {
            id: settings
        }

        FormCard.FormCheckDelegate {
            id: enable
            checked: settings.checkerEnabledByDefault
            text: i18n("Enable automatic spell checking")
            onCheckedChanged: {
                settings.checkerEnabledByDefault = checked;
                settings.save();
            }
        }

        FormCard.FormDelegateSeparator {
            below: enable; above: skipUppercase
        }

        FormCard.FormCheckDelegate {
            id: skipUppercase
            checked: settings.skipUppercase
            text: i18n("Ignore uppercase words")
            onCheckedChanged: {
                settings.skipUppercase = checked;
                settings.save();
            }
        }

        FormCard.FormDelegateSeparator {
            below: skipUppercase; above: skipRunTogether
        }

        FormCard.FormCheckDelegate {
            id: skipRunTogether
            checked: settings.skipRunTogether
            text: i18n("Ignore hyphenated words")
            onCheckedChanged: {
                settings.skipRunTogether = checked;
                settings.save();
            }
        }

        FormCard.FormDelegateSeparator {
            below: skipRunTogether; above: autodetectLanguageCheckbox
        }

        FormCard.FormCheckDelegate {
            id: autodetectLanguageCheckbox
            checked: settings.autodetectLanguage
            text: i18n("Detect language automatically")
            onCheckedChanged: {
                settings.autodetectLanguage = checked;
                settings.save();
            }
        }

        FormCard.FormDelegateSeparator {
            below: autodetectLanguageCheckbox; above: selectedDefaultLanguage
        }

        FormCard.FormComboBoxDelegate {
            id: selectedDefaultLanguage
            text: i18n("Selected default language:")
            model: isEmpty ? [{"display": i18n("None")}] : settings.dictionaryModel
            textRole: "display"
            displayMode: Kirigami.Settings.isMobile ? FormCard.FormComboBoxDelegate.Dialog : FormCard.FormComboBoxDelegate.Page
            valueRole: "languageCode"
            property bool isEmpty: false
            Component.onCompleted: {
                if (settings.dictionaryModel.rowCount() === 0) {
                    isEmpty = true;
                } else {
                    currentIndex = indexOfValue(settings.defaultLanguage);
                }
            }
            onActivated: settings.defaultLanguage = currentValue;
        }

        FormCard.FormDelegateSeparator {
            below: selectedDefaultLanguage; above: spellCheckingLanguage
        }

        FormCard.FormButtonDelegate {
            id: spellCheckingLanguage
            text: i18n("Additional Spell Checking Languages")
            description: i18n("%1 will provide spell checking and suggestions for the languages listed here when autodetection is enabled.", Qt.application.displayName)
            onClicked: pageStack.pushDialogLayer(spellCheckingLanguageList, {}, {
                width: pageStack.width - Kirigami.Units.gridUnit * 5,
                height: pageStack.height - Kirigami.Units.gridUnit * 5,
            })
        }

        FormCard.FormDelegateSeparator {
            below: spellCheckingLanguage; above: personalDictionary
        }

        FormCard.FormButtonDelegate {
            id: personalDictionary
            text: i18n("Open Personal Dictionary")
            onClicked: pageStack.pushDialogLayer(dictionaryPage, {}, {
                width: pageStack.width - Kirigami.Units.gridUnit * 5,
                height: pageStack.height - Kirigami.Units.gridUnit * 5,
            })
        }

        data: [
            Component {
                id: spellCheckingLanguageList
                Kirigami.ScrollablePage {
                    id: scroll
                    title: i18nc("@title:window", "Spell checking languages")
                    enabled: autodetectLanguageCheckbox.checked
                    ListView {
                        clip: true
                        model: settings.dictionaryModel
                        delegate: KirigamiDelegates.CheckSubtitleDelegate {
                            width: ListView.view.width

                            text: model.display
                            action: Kirigami.Action {
                                onTriggered: model.checked = checked
                            }
                            Accessible.description: model.isDefault ? i18n("Default Language") : ''
                            checked: model.checked

                            icon.source: model.isDefault ? "favorite" : undefined
                        }
                    }
                }
            },

            Component {
                id: dictionaryPage
                Kirigami.ScrollablePage {
                    title: i18n("Spell checking dictionary")
                    footer: QQC2.ToolBar {
                        contentItem: RowLayout {
                            QQC2.TextField {
                                id: dictionaryField
                                Layout.fillWidth: true
                                Accessible.name: placeholderText
                                placeholderText: i18n("Add a new word to your personal dictionary…")
                            }
                            QQC2.Button {
                                text: i18nc("@action:button", "Add Word")
                                icon.name: "list-add"
                                enabled: dictionaryField.text.length > 0
                                onClicked: {
                                    add(dictionaryField.text);
                                    dictionaryField.clear();
                                    if (instantApply) {
                                        settings.save();
                                    }
                                }
                                Layout.rightMargin: Kirigami.Units.largeSpacing
                            }
                        }
                    }
                    ListView {
                        topMargin: Math.round(Kirigami.Units.smallSpacing / 2)
                        bottomMargin: Math.round(Kirigami.Units.smallSpacing / 2)

                        model: settings.currentIgnoreList
                        delegate: Delegates.RoundedItemDelegate {
                            id: wordDelegate

                            required property var modelData

                            text: modelData

                            contentItem: RowLayout {
                                spacing: Kirigami.Units.smallSpacing

                                Delegates.DefaultContentItem {
                                    itemDelegate: wordDelegate
                                    Layout.fillWidth: true
                                }

                                QQC2.ToolButton {
                                    text: i18nc("@action:button", "Delete word")
                                    icon.name: "delete"
                                    display: QQC2.ToolButton.IconOnly
                                    onClicked: {
                                        remove(wordDelegate.modelData);
                                        if (instantApply) {
                                            settings.save();
                                        }
                                    }

                                    QQC2.ToolTip.text: text
                                    QQC2.ToolTip.visible: hovered
                                    QQC2.ToolTip.delay: Kirigami.ToolTip.toolTipDelay
                                }
                            }
                        }
                    }
                }
            }
        ]

        function add(word: string) {
            const dictionary = settings.currentIgnoreList;
            dictionary.push(word);
            settings.currentIgnoreList = dictionary;
        }

        function remove(word: string) {
            settings.currentIgnoreList = settings.currentIgnoreList.filter((value, _, _) => {
                return value !== word;
            });
        }
    }
}