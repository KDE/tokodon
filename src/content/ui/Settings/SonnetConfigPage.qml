// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami
import org.kde.sonnet 1.0 as Sonnet
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm

MobileForm.FormCard {
    Layout.topMargin: Kirigami.Units.largeSpacing
    Layout.fillWidth: true

    Sonnet.Settings {
        id: settings
    }

    contentItem: ColumnLayout {
        spacing: 0
        MobileForm.FormCardHeader {
            title: i18n("Spellchecking")
        }

        MobileForm.FormSwitchDelegate {
            id: enable
            checked: settings.checkerEnabledByDefault
            text: i18n("Enable automatic spell checking")
            onCheckedChanged: {
                settings.checkerEnabledByDefault = checked;
                settings.save();
            }
        }

        MobileForm.FormDelegateSeparator { below: enable; above: skipUppercase }

        MobileForm.FormSwitchDelegate {
            id: skipUppercase
            checked: settings.skipUppercase
            text: i18n("Ignore uppercase words")
            onCheckedChanged: {
                settings.skipUppercase = checked;
                settings.save();
            }
        }

        MobileForm.FormDelegateSeparator { below: skipUppercase; above: skipRunTogether }

        MobileForm.FormSwitchDelegate {
            id: skipRunTogether
            checked: settings.skipRunTogether
            text: i18n("Ignore hyphenated words")
            onCheckedChanged: {
                settings.skipRunTogether = checked;
                settings.save();
            }
        }

        MobileForm.FormDelegateSeparator { below: skipRunTogether; above: autodetectLanguageCheckbox }

        MobileForm.FormSwitchDelegate {
            id: autodetectLanguageCheckbox
            checked: settings.autodetectLanguage
            text: i18n("Detect language automatically")
            onCheckedChanged: {
                settings.autodetectLanguage = checked;
                settings.save();
            }
        }

        MobileForm.FormComboBoxDelegate {
            Kirigami.FormData.label: i18n("Selected default language:")
            model: settings.dictionaryModel
            textRole: "display"
            valueRole: "languageCode"
            Component.onCompleted: currentIndex = indexOfValue(settings.defaultLanguage);
            onActivated: {
                settings.defaultLanguage = currentValue;
            }
        }

        QQC2.Button {
            text: i18n("Open Personal Dictionary")
            onClicked: if (!dialog) {
                if (Kirigami.Settings.isMobile) {
                    dialog = mobileSheet.createObject(page, {settings: settings});
                    dialog.open();
                } else {
                    dialog = desktopSheet.createObject(page, {settings: settings})
                    dialog.show();
                }
            } else {
                if (Kirigami.Settings.isMobile) {
                    dialog.open();
                } else {
                    dialog.show();
                }
            }
        }
    }

    QQC2.Dialog {
        id: applyDialog
        title: qsTr("Apply Settings")
        contentItem: QQC2.Label {
            text: qsTr("The settings of the current module have changed.<br /> Do you want to apply the changes or discard them?")
        }
        standardButtons: QQC2.Dialog.Ok | QQC2.Dialog.Cancel | QQC2.Dialog.Discard

        onAccepted: {
            settings.save();
            applyDialog.close();
            page.close();
        }
        onDiscarded: {
            applyDialog.close();
            page.close();
        }
        onRejected: applyDialog.close();
    }

    property var dialog: null

/*
    ColumnLayout {
        anchors.fill: parent

        Kirigami.FormLayout {
            Layout.fillWidth: true
            Layout.leftMargin: wideMode ? 0 : Kirigami.Units.largeSpacing
            Layout.rightMargin: wideMode ? 0 : Kirigami.Units.largeSpacing


        }

        Kirigami.Heading {
            level: 2
            text: i18n("Spell checking languages")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.leftMargin: wideMode ? 0 : Kirigami.Units.largeSpacing
            Layout.rightMargin: wideMode ? 0 : Kirigami.Units.largeSpacing
        }
        QQC2.Label {
            text: i18n("%1 will provide spell checking and suggestions for the languages listed here when autodetection is enabled.", Qt.application.displayName)
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.leftMargin: wideMode ? 0 : Kirigami.Units.largeSpacing
            Layout.rightMargin: wideMode ? 0 : Kirigami.Units.largeSpacing
        }

        QQC2.ScrollView {
            id: scroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            enabled: autodetectLanguageCheckbox.checked
            Component.onCompleted: background.visible = wideMode
            ListView {
                clip: true
                model: settings.dictionaryModel
                delegate: Kirigami.CheckableListItem {
                    label: model.display
                    action: Kirigami.Action {
                        onTriggered: model.checked = checked
                    }
                    checked: model.checked
                    trailing: Kirigami.Icon {
                        source: "favorite"
                        visible: model.isDefault
                        HoverHandler {
                            id: hover
                        }
                        QQC2.ToolTip {
                            visible: hover.hovered
                            text: qsTr("Default Language")
                        }
                    }
                }
            }
        }
    }

    component SheetHeader : RowLayout {
        QQC2.TextField {
            id: dictionaryField
            Layout.fillWidth: true
            placeholderText: i18n("Add a new word to your personal dictionary…")
        }
        QQC2.Button {
            text: i18nc("@action:button", "Add word")
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

    Component {
        id: desktopSheet
        QQC2.ApplicationWindow {
            id: window
            required property Sonnet.Settings settings
            title: i18n("Spell checking dictionary")
            width: Kirigami.Units.gridUnit * 20
            height: Kirigami.Units.gridUnit * 20
            flags: Qt.Dialog | Qt.WindowCloseButtonHint
            header: Kirigami.AbstractApplicationHeader {
                leftPadding: Kirigami.Units.smallSpacing
                rightPadding: Kirigami.Units.smallSpacing
                contentItem: SheetHeader {
                    anchors.fill: parent
                }
            }
            QQC2.ScrollView {
                anchors.fill: parent
                ListView {
                    model: settings.currentIgnoreList
                    delegate: Kirigami.BasicListItem {
                        label: model.modelData
                        trailing: QQC2.ToolButton {
                            icon.name: "delete"
                            onClicked: {
                                remove(modelData)
                                if (instantApply) {
                                    settings.save();
                                }
                            }
                            QQC2.ToolTip {
                                text: i18n("Delete word")
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: mobileSheet
        Kirigami.OverlaySheet {
            required property Sonnet.Settings settings
            id: dictionarySheet

            header: SheetHeader {}

            ListView {
                implicitWidth: Kirigami.Units.gridUnit * 15
                model: settings.currentIgnoreList
                delegate: Kirigami.BasicListItem {
                    label: model.modelData
                    trailing: QQC2.ToolButton {
                        icon.name: "delete"
                        onClicked: {
                            remove(modelData)
                            if (instantApply) {
                                settings.save();
                            }
                        }
                        QQC2.ToolTip {
                            text: i18n("Delete word")
                        }
                    }
                }
            }
        }
    }

    footer: QQC2.ToolBar {
        visible: !instantApply
        height: visible ? implicitHeight : 0
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }

            QQC2.Button  {
                text: i18n("Apply")
                enabled: settings.modified
                onClicked: settings.save();
            }
        }
    }

    function add(word) {
        const dictionary = settings.currentIgnoreList;
        dictionary.push(word);
        settings.currentIgnoreList = dictionary;
    }

    function remove(word) {
        settings.currentIgnoreList = settings.currentIgnoreList.filter(function (value, _, _) {
            return value !== word;
        });
    }

    */
}
