// SPDX-FileCopyrightText: 2024 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kitemmodels
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as Components

Kirigami.ScrollablePage {
    id: root

    property alias model: searchFilterProxyModel.sourceModel

    title: i18ndc("kirigami-addons6", "@title:window", "Shortcuts")

    actions: Kirigami.Action {
        displayComponent: Kirigami.SearchField {
            placeholderText: i18ndc("kirigami-addons6", "@label:textbox", "Filter…")
            onTextChanged: searchFilterProxyModel.setFilterFixedString(text);
        }
    }

    ListView {
        id: listView

        currentIndex: -1

        model: KSortFilterProxyModel {
            id: searchFilterProxyModel
            sourceModel: ActionModel {
                id: actionModel
                // TODO: this should just fetch all actions of all collections
                collectionName: "tokodon_actions"
                shownActions: ActionModel.ActiveActions
            }

            filterRoleName: 'actionName'
            filterCaseSensitivity: Qt.CaseInsensitive
        }

        delegate: QQC2.ItemDelegate {
            id: shortcutDelegate

            required property int index
            required property QtObject actionDescription
            required property QtObject actionInstance

            width: ListView.view.width
            text: actionDescription.text//actionName.replace('&', '')

            Accessible.description: actionDescription.shortcut

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: implicitWidth
                    source: actionDescription.icon
                }

                QQC2.Label {
                    text: actionDescription.text
                    Layout.fillWidth: true
                    Accessible.ignored: true
                }

                QQC2.Label {
                    text: actionDescription?.shortcut
                    Accessible.ignored: true
                }
            }

            onClicked: {
                shortcutDialog.title = i18ndc("kirigami-addons6", "@title:window", "Shortcut: %1",  actionDescription.text);
                shortcutDialog.actionDescription = actionDescription;
                shortcutDialog.keySequence = actionDescription.shortcut;
                shortcutDialog.index = shortcutDelegate.index;
                //shortcutDialog.alternateShortcuts = shortcutDelegate.alternateShortcuts;
                shortcutDialog.open()
            }
        }

        FormCard.FormCardDialog {
            id: shortcutDialog

            property ActionData actionDescription
            property alias keySequence: keySequenceItem.keySequence
            property var alternateShortcuts
            property int index: -1

            parent: root.QQC2.Overlay.overlay

            KeySequenceItem {
                id: keySequenceItem

                label: i18ndc("kirigami-addons6", "@label", "Shortcut:")
                onKeySequenceModified: {
                    shortcutDialog.actionDescription.shortcut = keySequence;
                }

                onErrorOccurred: (title, message) => {
                    root.QQC2.ApplicationWindow.showPassiveNotification(title + '\n' + message);
                }

                onShowStealStandardShortcutDialog: (title, message, sequence) => {
                    stealStandardShortcutDialog.title = title
                    stealStandardShortcutDialog.message = message;
                    stealStandardShortcutDialog.sequence = sequence;
                    stealStandardShortcutDialog.parent = root.QQC2.Overlay.overlay;
                    stealStandardShortcutDialog.sequenceItem = this;
                    stealStandardShortcutDialog.openDialog();
                }
            }

            Components.MessageDialog {
                id: stealStandardShortcutDialog

                property string message
                property var sequence
                property KeySequenceItem sequenceItem

                dialogType: Components.MessageDialog.Warning
                dontShowAgainName: "stealStandardShortcutDialog"

                QQC2.Label {
                    text: stealStandardShortcutDialog.message
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }

                standardButtons: Kirigami.PromptDialog.Apply | Kirigami.PromptDialog.Cancel

                onApplied: {
                    sequenceItem.stealStandardShortcut(sequence);
                    close();
                }

                onRejected: close()
            }

            Repeater {
                id: alternateRepeater

                //model: shortcutDialog.actionDescription.shortcuts
                KeySequenceItem {
                    id: alternateKeySequenceItem

                    required property int index
                    required property var modelData

                    label: index === 0 ? i18ndc("kirigami-addons6", "@label", "Alternative:") : ''

                    keySequence: modelData
                    onKeySequenceModified: {
                        const alternates = root.model.updateShortcut(shortcutDialog.index, index + 1, keySequence);
                        if (alternates !== shortcutDialog.actionDescription.shortcuts) {
                            shortcutDialog.actionDescription.shortcuts = alternates;
                        }
                    }

                    onErrorOccurred: (title, message) => {
                        root.QQC2.ApplicationWindow.showPassiveNotification(title + '\n' + message);
                    }

                    onShowStealStandardShortcutDialog: (title, message, sequence) => {
                        stealStandardShortcutDialog.title = title
                        stealStandardShortcutDialog.message = message;
                        stealStandardShortcutDialog.sequence = sequence;
                        stealStandardShortcutDialog.parent = root.QQC2.Overlay.overlay;
                        stealStandardShortcutDialog.sequenceItem = this;
                        stealStandardShortcutDialog.openDialog();
                    }
                }
            }

            KeySequenceItem {
                id: alternateKeySequenceItem

                label: alternateRepeater.count === 0 ? i18ndc("kirigami-addons6", "@label", "Alternative:") : ''

                onKeySequenceModified: {
                    //shortcutDialog.alternateShortcuts = root.model.updateShortcut(shortcutDialog.index, alternateRepeater.count + 1, keySequence);
                    shortcutDialog.actionDescription.shortcuts = keySequence;
                    //keySequence = root.model.emptyKeySequence();
                }

                onErrorOccurred: (title, message) => {
                    root.QQC2.ApplicationWindow.showPassiveNotification(title + '\n' + message);
                }

                onShowStealStandardShortcutDialog: (title, message, sequence) => {
                    stealStandardShortcutDialog.title = title
                    stealStandardShortcutDialog.message = message;
                    stealStandardShortcutDialog.sequence = sequence;
                    stealStandardShortcutDialog.parent = root.QQC2.Overlay.overlay;
                    stealStandardShortcutDialog.sequenceItem = this;
                    stealStandardShortcutDialog.openDialog();
                }
            }

            footer: RowLayout {
                QQC2.DialogButtonBox {
                    Layout.fillWidth: true
                    standardButtons: QQC2.DialogButtonBox.Close | QQC2.DialogButtonBox.Reset
                    onRejected: shortcutDialog.close();
                    onReset: shortcutDialog.alternateShortcuts = root.model.reset(shortcutDialog.index)
                    leftPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                    topPadding: Kirigami.Units.smallSpacing
                    rightPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                    bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                }
            }
        }

        Kirigami.PlaceholderMessage {
            width: parent.width - Kirigami.Units.gridUnit * 4
            anchors.centerIn: parent
            text: i18ndc("kirigami-addons6", "Placeholder message", "No shortcuts found")
            visible: listView.count === 0
        }
    }

    footer: QQC2.ToolBar {
        padding: 0

        contentItem: QQC2.DialogButtonBox {
            padding: Kirigami.Units.largeSpacing
            standardButtons: QQC2.Dialog.Save | QQC2.Dialog.Reset

            onAccepted: {
                root.model.save()
                root.closeDialog();
            }
            onReset: root.model.resetAll()
        }
    }
}
