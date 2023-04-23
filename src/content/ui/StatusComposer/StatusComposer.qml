// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0
import '..'

MastoPage {
    id: root

    enum Purpose {
        New,
        Reply,
        Redraft,
        Edit
    }

    property var purpose
    property string inReplyTo: ''
    property var mentions: []
    property int visibility: AccountManager.selectedAccount.preferences.defaultVisibility
    property int sensitive: AccountManager.selectedAccount.preferences.defaultSensitive
    readonly property NetworkRequestProgress progress: NetworkRequestProgress {}
    property var previewPost: null

    readonly property PostEditorBackend defaultBackend: PostEditorBackend {
        inReplyTo: root.inReplyTo
        mentions: root.mentions
        visibility: root.visibility
        sensitive: root.sensitive
    }

    property PostEditorBackend backend: defaultBackend

    readonly property bool isPollValid: backend.pollEnabled ? backend.poll.isValid : true

    title: {
        switch (root.purpose) {
            case StatusComposer.Edit:
                return i18n("Edit this post")
            case StatusComposer.Reply:
                return i18n("Reply to this post")
            case StatusComposer.Redraft:
                return i18n("Rewrite this post")
            case StatusComposer.New:
                return i18n("Write a new post")
        }
    }

    Connections {
        target: backend
        function onPosted(error) {
            if (error.length === 0) {
                applicationWindow().pageStack.layers.pop();
            } else {
                inlineMessage.text = error
                console.log(error);
            }
        }

        function onEditComplete(obj) {
            applicationWindow().pageStack.layers.pop();
        }
    }

    Component.onCompleted: textArea.forceActiveFocus()

    function submitPost() {
        if(root.purpose === StatusComposer.Edit) {
            backend.edit()
        } else {
            backend.save()
        }
    }

    Kirigami.FlexColumn {
        maximumWidth: Kirigami.Units.gridUnit * 30
        padding: 0

        Kirigami.InlineMessage {
            id: inlineMessage
            Layout.fillWidth: true
            visible: text.length !== 0
            type: Kirigami.MessageType.Error
        }

        QQC2.TextField {
            placeholderText: i18n("Content Warning")
            Layout.fillWidth: true
            visible: contentWarning.checked
            onTextChanged: root.backend.spoilerText = text
        }

        Loader {
            active: root.previewPost !== null
            Layout.fillWidth: true
            sourceComponent: StatusPreview {
                post: root.previewPost
            }
        }

        QQC2.TextArea {
            id: textArea
            placeholderText: i18n("What's new?")
            text: root.backend.status
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 12 + actions.implicitHeight
            bottomInset: -1
            leftInset: -1
            rightInset: -1
            onTextChanged: backend.status = text
            Kirigami.SpellChecking.enabled: true

            Component.onCompleted: {
                if (root.purpose === StatusComposer.New || root.purpose === StatusComposer.Reply) {
                    textArea.text = root.backend.mentions.filter((mention) => mention !== ('@' + AccountManager.selectedAccount.identity.account)).join(" ")
                }
            }

            Keys.onEnterPressed: (event)=> {
               if (event.modifiers & Qt.ControlModifier) {
                    root.submitPost()
                    event.accepted = true
               } else {
                    event.accepted = false
               }
            }

            Keys.onReturnPressed: (event)=> {
               if (event.modifiers & Qt.ControlModifier) {
                    root.submitPost()
                    event.accepted = true
               } else {
                    event.accepted = false
               }
            }

            ColumnLayout {
                id: actions
                spacing: 0
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    leftMargin: 1
                    rightMargin: 1
                    bottomMargin: 1
                }
                height: implicitHeight

                Behavior on height {
                    NumberAnimation {
                        property: "height"
                        duration: Kirigami.Units.shortDuration
                        easing.type: Easing.OutCubic
                    }
                }

                AttachmentGrid {
                    attachmentEditorModel: root.backend.attachmentEditorModel
                    Layout.fillWidth: true
                    Layout.margins: Kirigami.Units.smallSpacing
                }

                QQC2.ProgressBar {
                    Layout.fillWidth: true
                    from: 0
                    to: 100
                    visible: progress.uploading
                    value: progress.progress
                    indeterminate: progress.progress === 100 && progress.uploading
                    Layout.leftMargin: Kirigami.Units.smallSpacing
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                }

                Kirigami.Separator {
                    visible: addPool.checked
                    Layout.fillWidth: true
                }

                Poll {
                    visible: addPool.checked
                    Layout.fillWidth: true

                    poll: backend.poll
                }

                QQC2.ToolBar {
                    id: actionsToolbar

                    Layout.fillWidth: true

                    RowLayout {
                        QQC2.ToolButton {
                            enabled: backend.attachmentEditorModel.count < 4 && !addPool.checked
                            icon.name: "mail-attachment-symbolic"
                            onClicked: fileDialog.open()
                            FileDialog {
                                id: fileDialog
                                folder: shortcuts.home
                                title: i18n("Please choose a file")
                                onAccepted: progress.reply = backend.attachmentEditorModel.append(fileDialog.fileUrl);
                            }
                            QQC2.ToolTip.text: i18n("Attach File")
                            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                            QQC2.ToolTip.visible: hovered
                        }

                        QQC2.ToolButton {
                            id: addPool
                            icon.name: "gnumeric-graphguru"
                            checkable: true
                            enabled: backend.attachmentEditorModel.count === 0 && root.purpose !== StatusComposer.Edit
                            QQC2.ToolTip.text: i18n("Add Poll")
                            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                            QQC2.ToolTip.visible: hovered

                            onToggled: backend.pollEnabled = checked
                        }

                        QQC2.ToolButton {
                            icon.name: {
                                switch(backend.visibility) {
                                    case Post.Public:
                                        return "kstars_xplanet";
                                    case Post.Unlisted:
                                        return "unlock";
                                    case Post.Private:
                                        return "lock";
                                    case Post.Direct:
                                        return "mail-message";
                                    default:
                                        return "kstars_xplanet";
                                }
                            }
                            onClicked: visibilityMenu.open()
                            enabled: root.purpose !== StatusComposer.Edit
                            QQC2.Menu {
                                id: visibilityMenu
                                QQC2.MenuItem {
                                    icon.name: "kstars_xplanet"
                                    text: i18n("Public")
                                    onTriggered: backend.visibility = Post.Public
                                }
                                QQC2.MenuItem {
                                    icon.name: "unlock"
                                    text: i18n("Unlisted")
                                    onTriggered: backend.visibility = Post.Unlisted
                                }
                                QQC2.MenuItem {
                                    icon.name: "lock"
                                    text: i18n("Private")
                                    onTriggered: backend.visibility = Post.Private
                                }
                                QQC2.MenuItem {
                                    icon.name: "mail-message"
                                    text: i18n("Direct Message")
                                    onTriggered: backend.visibility = Post.Direct
                                }
                            }
                            QQC2.ToolTip {
                                text: i18n("Visibility")
                            }
                        }
                        QQC2.ToolButton {
                            id: contentWarning
                            text: i18nc("Short for content warning", "cw")
                            checkable: true
                            QQC2.ToolTip {
                                text: i18n("Content Warning")
                            }
                        }
                        LanguageSelector {
                            id: languageSelect

                            Accessible.name: QQC2.ToolTip.text
                            Accessible.description: i18nc("@info:whatsthis Post language selection", "Select the language the post is written in")

                            Component.onCompleted: currentIndex = indexOfValue(backend.language);
                            onActivated: backend.language = model.getCode(currentIndex)
                            ;

                            QQC2.ToolTip.text: i18nc("@label:listbox Post language selection", "Post Language")
                            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                            QQC2.ToolTip.visible: hovered
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true

            QQC2.Label {
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft
                text: root.backend.charactersLeft
            }

            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                text: i18n("Send")
                enabled: textArea.text.length > 0 && isPollValid && (!progress.uploading || backend.attachmentEditorModel.count > 0)
                Layout.alignment: Qt.AlignRight
                onClicked: root.submitPost()
            }
        }
    }
}
