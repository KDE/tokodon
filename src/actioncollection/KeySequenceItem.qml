// SPDX-FileCopyrightText: 2014 David Edmundson <kde@davidedmundson.co.uk>
// SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>
// SPDX-FileCopyrightText: 2022 Aleix Pol <aleixpol@kde.org>
// SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.statefulapp.private
import org.kde.kirigamiaddons.formcard as FormCard

/**
 * @internal
 */
FormCard.AbstractFormDelegate {
    id: root

    required property string label
    property bool showClearButton: true
    property bool showCancelButton: true
    property alias modifierOnlyAllowed: helper.modifierOnlyAllowed
    property alias modifierlessAllowed: helper.modifierlessAllowed
    property alias multiKeyShortcutsAllowed: helper.multiKeyShortcutsAllowed
    property alias keySequence: helper.currentKeySequence

    /**
     * This property controls which types of shortcuts are checked for conflicts when the keySequence
     * is set. If a conflict is detected, a messagebox will be shown asking the user to confirm their
     * input. Valid values are combinations of the following flags:
     *   - @p ShortcutType.None Do not check for conflicts.
     *   - @p ShortcutType.StandardShortcuts Check against standard shortcuts. @see KStandardShortcut
     *   - @p ShortcutType.GlobalShortcuts Check against global shortcuts. Only works when building with KGlobalAccel. @see KGlobalAccel
     *
     * The default is `ShortcutType.GlobalShortcuts | ShortcutType.StandardShortcut`
     */
    property alias checkForConflictsAgainst: helper.checkAgainstShortcutTypes

    property string __previousSequence: ""

    /**
     * Emitted whenever the key sequence is modified by the user interacting with the component
     *
     * Either by interacting capturing a key sequence or pressing the clear button.
     */
    signal keySequenceModified()

    signal errorOccurred(title: string, message: string)

    signal showStealStandardShortcutDialog(title: string, message: string, sequence: var)

    /**
     * Start capturing a key sequence. This equivalent to the user clicking on the main button of the item.
     */
    function startCapturing() {
        mainButton.checked = true;
    }

    function stealStandardShortcut(sequence): void {
        applyStealStandardShortcutTimer.sequence = sequence;
        applyStealStandardShortcutTimer.start();
    }

    Timer {
        id: applyStealStandardShortcutTimer

        property var sequence

        interval: 200
        onTriggered: {
            helper.currentKeySequence = sequence;
            root.keySequenceModified();
        }
    }

    background: null

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        QQC2.Label {
            text: root.label
            visible: text.length > 0
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true

            KeySequenceHelper {
                id: helper
                onGotKeySequence: keySequence => {
                    if (!isKeySequenceAvailable(keySequence)) {
                        currentKeySequence = root.__previousSequence;
                    }
                    mainButton.checked = false;
                    root.keySequenceModified();
                }

                onErrorOccurred: (title, message) => {
                    root.errorOccurred(title, message);
                }

                onShowStealStandardShortcutDialog: (title, message, seq) => {
                    root.showStealStandardShortcutDialog(title, message, seq);
                }
            }

            QQC2.Button {
                id: mainButton

                Layout.fillHeight: true
                Layout.fillWidth: true

                icon.name: "configure"

                checkable: true
                focus: checked

                hoverEnabled: true

                text: {
                    const keySequence = helper.currentKeySequence;
                    const text = helper.keySequenceIsEmpty(keySequence)
                        ? (helper.isRecording
                            ? i18ndc("kirigami-addons6", "What the user inputs now will be taken as the new shortcut", "Input")
                            : i18ndc("kirigami-addons6", "No shortcut defined", "None"))
                        // Single ampersand gets interpreted by the button as a mnemonic
                        // and removed; replace it with a double ampersand so that it
                        // will be displayed by the button as a single ampersand, or
                        // else shortcuts with the actual ampersand character will
                        // appear to be partially empty.
                        : helper.keySequenceNativeText(keySequence).replace('&', '&&');
                    // These spaces are intentional
                    return " " + text + (helper.isRecording ? " ... " : " ");
                }

                Accessible.description: i18nd("kirigami-addons6", "Click on the button, then enter the shortcut like you would in the program.\nExample for Ctrl+A: hold the Ctrl key and press A.")

                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.text: Accessible.description
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                onCheckedChanged: {
                    if (checked) {
                        root.__previousSequence = helper.keySequenceNativeText(root.keySequence)
                        helper.window = helper.renderWindow(parent.Window.window)
                        mainButton.forceActiveFocus()
                        helper.startRecording()
                    } else if (helper.isRecording) {
                        helper.cancelRecording()
                    }
                }

                onFocusChanged: {
                    if (!focus) {
                        mainButton.checked = false
                    }
                }
            }

            QQC2.Button {
                id: clearButton
                Layout.fillHeight: true
                Layout.preferredWidth: height
                visible: root.showClearButton && !helper.isRecording
                onClicked: {
                    root.keySequence = "";
                    root.keySequenceModified();
                }

                enabled: !helper.keySequenceIsEmpty(helper.currentKeySequence)

                hoverEnabled: true
                // icon name determines the direction of the arrow, NOT the direction of the app layout
                icon.name: Qt.application.layoutDirection === Qt.LeftToRight ? "edit-clear-locationbar-rtl" : "edit-clear-locationbar-ltr"

                display: QQC2.Button.IconOnly
                text: i18ndc("kirigami-addons6", "@info:tooltip", "Clear Key Sequence")

                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.text: text
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }

            QQC2.Button {
                Layout.fillHeight: true
                Layout.preferredWidth: height
                onClicked: helper.cancelRecording()
                visible: root.showCancelButton && helper.isRecording

                icon.name: "dialog-cancel"

                display: QQC2.Button.IconOnly
                text: i18ndc("kirigami-addons6", "@info:tooltip", "Cancel Key Sequence Recording")

                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.text: Accessible.name
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
        }
    }
}
