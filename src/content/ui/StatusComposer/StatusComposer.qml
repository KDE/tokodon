// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2022 Joshua Goins <josh@redstrate.com>
// SPDX-FileCopyrightText: 2022 Jeremy Winter <jeremy.winter@tutanota.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtCore
import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Window
import QtQuick.Effects
import org.kde.kirigami 2 as Kirigami
import org.kde.kquickcontrolsaddons as KQuickControlsAddons
import org.kde.tokodon
import org.kde.tokodon.private
import '..'

Kirigami.ScrollablePage {
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
    property string initialText
    property bool closeApplicationWhenFinished: false
    property bool discardDraft: false
    property string contentWarning: 'test'

    readonly property PostEditorBackend defaultBackend: PostEditorBackend {
        inReplyTo: root.inReplyTo
        mentions: root.mentions
        visibility: root.visibility
        sensitive: root.sensitive
    }

    property PostEditorBackend backend: defaultBackend

    readonly property bool isPollValid: backend.pollEnabled ? backend.poll.isValid : true
    readonly property bool isStatusValid: textArea.text.length > 0 && backend.charactersLeft >= 0

    title: {
        switch (root.purpose) {
            case StatusComposer.Edit:
                return i18nc("@title:window", "Edit This Post")
            case StatusComposer.Reply:
                return i18nc("@title:window", "Reply to This Post")
            case StatusComposer.Redraft:
                return i18nc("@title:window", "Rewrite This Post")
            case StatusComposer.New:
                return i18nc("@title:window", "Write a New Post")
        }
    }

    actions: [
        Kirigami.Action {
            text: i18nc("@action Pop out the status composer", "Pop Out")
            icon.name: "window-new-symbolic"
            visible: !Kirigami.Settings.isMobile && !root.closeApplicationWhenFinished
            onTriggered: {
                pageStack.layers.pop();
                applicationWindow().popoutStatusComposer(root);
            }
        }
    ]

    data: Connections {
        target: backend
        function onPosted(error) {
            if (error.length === 0) {
                root.discardDraft = true;
                if (root.closeApplicationWhenFinished) {
                    root.Window.window.close();
                } else {
                    root.Window.window.pageStack.layers.pop();
                }
                applicationWindow().newPost();
            } else {
                banner.type = Kirigami.MessageType.Error;
                banner.text = error;
                console.log(error);
            }
        }

        function onEditComplete(obj) {
            if (root.closeApplicationWhenFinished) {
                root.Window.window.close();
            } else {
                root.Window.window.pageStack.layers.pop();
            }
        }
    }

    Component.onCompleted: {
        if (initialText.length > 0) {
            backend.status = initialText
        }

        textArea.forceActiveFocus()
    }

    // Currently only used for pop-out, but could be repurposed for loading drafts
    function refreshData() {
        if (root.backend.spoilerText.length > 0) {
            contentWarningField.text = root.backend.spoilerText;
            contentWarning.checked = true;
        }

        // Stop it from overwriting existing text
        if (textArea.text === "") {
            const filteredMentions = root.backend.mentions.filter((mention) => mention !== ('@' + AccountManager.selectedAccount.identity.account));
            if (filteredMentions.length > 0) {
                textArea.text = filteredMentions.join(" ") + " ";
                textArea.cursorPosition = textArea.length;
            }
        }

        if (root.purpose === StatusComposer.Edit && root.backend.pollEnabled) {
            banner.type = Kirigami.MessageType.Warning;
            banner.text = i18n("Saving an edited post that has a poll will clear existing results.");
        }
    }

    Kirigami.PromptDialog {
        id: discardDraftPrompt

        title: i18nc("@title", "Discard Draft")
        subtitle: i18nc("@label", "Are you sure you want to discard your draft?")
        standardButtons: Kirigami.Dialog.Discard
        showCloseButton: false

        customFooterActions: [
             Kirigami.Action {
                 text: i18nc("@action:button Keep this draft", "Keep")
                 icon.name: "dialog-cancel-symbolic"
                 onTriggered: discardDraftPrompt.close()
             }
        ]

        onDiscarded: {
            root.discardDraft = true;
            if (root.closeApplicationWhenFinished) {
                root.Window.window.close();
            } else {
                applicationWindow().pageStack.layers.pop();
            }
        }
    }

    onBackRequested: (event) => {
        if (shouldClose()) {
            event.accepted = true;
        }
    }

    function shouldClose(): bool {
        if (textArea.text.length > 0 && !root.discardDraft) {
            discardDraftPrompt.parent = root.Window.window.overlay; // workaround Kirigami.PromptDialog being broken
            discardDraftPrompt.open();

            return true;
        }

        return false;
    }

    function submitPost() {
        if(root.purpose === StatusComposer.Edit) {
            backend.edit()
        } else {
            backend.save()
        }
    }

    function uploadFile(url: string): void {
        progress.reply = backend.attachmentEditorModel.append(url);
    }

    function uploadData(data: var): void {
        progress.reply = backend.attachmentEditorModel.appendData(data);
    }

    function pasteImage(): bool {
        // First let's check if there's image data in the clipboard we can use.
        // Despite this being named image/png, lots of applications will provide image/png even if the underlying type is something else.
        if (clipboard.formats.includes("image/png")) {
            uploadData(clipboard.contentFormat("image/png"));
            return true;
        }

        // Otherwise, if it's a local path and the system failed to give us image data, try to upload that.
        let localPath = clipboard.content;
        if (localPath.length === 0) {
            return false;
        }
        if (backend.attachmentEditorModel.isLocalFile(localPath)) {
            uploadFile(localPath);
            return true;
        }

        // In the last case, paste it anyway to prevent it from disappearing into the void
        return false;
    }

    KQuickControlsAddons.Clipboard { id: clipboard }

    header: Kirigami.InlineMessage {
        id: banner
        Layout.fillWidth: true
        width: parent.width
        visible: text.length !== 0
        type: Kirigami.MessageType.Error
        position: Kirigami.InlineMessage.Position.Header
    }

    Kirigami.FlexColumn {
        maximumWidth: Kirigami.Units.gridUnit * 40
        padding: 0

        Loader {
            active: root.previewPost !== null
            Layout.fillWidth: true
            sourceComponent: StatusPreview {
                post: root.previewPost
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.smallSpacing * 2

            visible: root.previewPost !== null
        }

        InlineIdentityInfo {
            identity: AccountManager.selectedAccount.identity
            secondary: false

            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.smallSpacing * 2
        }

        QQC2.TextField {
            id: contentWarningField

            placeholderText: i18nc("@info:placeholder", "Content notice")
            Layout.fillWidth: true
            visible: contentWarning.checked
            font.pixelSize: Config.defaultFont.pixelSize + 1
            onTextChanged: {
                // Only update the backend if the content warning checkbox is wanted
                if (contentWarning.checked) {
                    root.backend.spoilerText = text
                }
            }
        }

        QQC2.TextArea {
            id: textArea
            placeholderText: i18nc("@info:placeholder", "What's new?")
            text: root.backend.status
            font.pixelSize: Config.defaultFont.pixelSize + 1
            wrapMode: TextEdit.Wrap
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: Math.max(implicitHeight, Kirigami.Units.gridUnit * 12) + actions.implicitHeight
            bottomInset: -1
            leftInset: -1
            rightInset: -1
            onTextChanged: backend.status = text

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

            Keys.onTabPressed: (event)=> {
                nextItemInFocusChain(true).forceActiveFocus(Qt.TabFocusReason)
                event.accepted = true
            }

            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_V && event.modifiers & Qt.ControlModifier) {
                    event.accepted = root.pasteImage()
                }
            }

            DropArea {
                anchors.fill: parent
                enabled: !AccountManager.isFlatpak
                onDropped: (drop) => {
                    for (let index in drop.urls) {
                        root.uploadFile(drop.urls[index])
                    }
                }
            }

            ColumnLayout {
                id: actions
                spacing: 0
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: implicitHeight

                Behavior on height {
                    NumberAnimation {
                        property: "height"
                        duration: Kirigami.Units.shortDuration
                        easing.type: Easing.OutCubic
                    }
                }

                EditorAttachmentGrid {
                    attachmentEditorModel: root.backend.attachmentEditorModel
                    Layout.fillWidth: true
                    Layout.maximumWidth: actions.width - Layout.leftMargin - Layout.rightMargin - (columns > 1 ? Kirigami.Units.smallSpacing : 0)
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

                ComposerPoll {
                    visible: addPool.checked && root.purpose !== StatusComposer.Edit
                    Layout.fillWidth: true

                    poll: backend.poll
                    maxPollOptions: AccountManager.selectedAccount.maxPollOptions
                }

                QQC2.ToolBar {
                    id: actionsToolbar

                    position: QQC2.ToolBar.Footer

                    // To make sure the corners are rounded like the text area it's sitting in
                    layer.enabled: true
                    layer.effect: MultiEffect {
                        id: rootEffect

                        maskEnabled: true
                        maskSpreadAtMax: 1
                        maskSpreadAtMin: 1
                        maskThresholdMin: 0.5
                        maskSource: ShaderEffectSource {
                            sourceItem: Kirigami.ShadowedRectangle {
                                width: rootEffect.width
                                height: rootEffect.height
                                corners {
                                    bottomLeftRadius: Kirigami.Units.cornerRadius
                                    bottomRightRadius: Kirigami.Units.cornerRadius
                                }
                            }
                        }
                    }

                    Layout.fillWidth: true

                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.ToolButton {
                            enabled: backend.attachmentEditorModel.count < 4 && !addPool.checked
                            icon.name: "mail-attachment-symbolic"
                            onClicked: fileDialog.open()
                            FileDialog {
                                id: fileDialog
                                currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
                                title: i18nc("@title:window", "Choose a File")
                                onAccepted: root.uploadFile(fileDialog.selectedFile);
                                selectedNameFilter.index: 0
                                nameFilters: [i18n("All supported formats (*.jpg *.jpeg *.png *.gif *.webp *.heic *.heif *.avif *.webm *.mp4 *.m4v *.mov)"),
                                    i18n("JPEG image (*.jpg *.jpeg)"),
                                    i18n("PNG image (*.png)"),
                                    i18n("GIF image (*.gif)"),
                                    i18n("WebP image (*.webp)"),
                                    i18n("HEIC image(*.heic)"),
                                    i18n("HEIF image (*.heif)"),
                                    i18n("AVIF image (*.avif)"),
                                    i18n("WebM video (*.webm)"),
                                    i18n("MPEG-4 video (*.mp4)"),
                                    i18n("M4V video (*.m4v)"),
                                    i18n("QuickTime video (*.mov)"),
                                    i18n("All files (*)")]
                            }
                            QQC2.ToolTip.text: i18nc("@info:tooltip", "Attach file")
                            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                            QQC2.ToolTip.visible: hovered
                        }

                        QQC2.ToolButton {
                            id: addPool
                            icon.name: "gnumeric-graphguru"
                            checkable: true
                            checked: backend.pollEnabled
                            enabled: backend.attachmentEditorModel.count === 0 && root.purpose !== StatusComposer.Edit
                            QQC2.ToolTip.text: i18nc("@info:tooltip", "Add poll")
                            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                            QQC2.ToolTip.visible: hovered

                            onToggled: backend.pollEnabled = checked
                        }

                        QQC2.ToolButton {
                            id: contentWarning
                            icon.name: "view-hidden-symbolic"
                            checkable: true
                            QQC2.ToolTip {
                                text: i18nc("@info:tooltip", "Content Notice")
                            }
                            onCheckedChanged: {
                                // Clear existing content warning if not checked
                                if (!checked) {
                                    root.backend.spoilerText = "";
                                } else {
                                    root.backend.spoilerText = contentWarningField.text;
                                }
                            }
                        }

                        QQC2.ToolSeparator {}

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
                                    case Post.Local:
                                        return "group";
                                    default:
                                        return "kstars_xplanet";
                                }
                            }
                            onClicked: visibilityMenu.open()
                            enabled: root.purpose !== StatusComposer.Edit
                            QQC2.Menu {
                                id: visibilityMenu
                                QQC2.MenuItem {
                                    icon.name: "group"
                                    text: i18nc("@item:inmenu Local to this server", "Local")
                                    onTriggered: backend.visibility = Post.Local
                                    visible: AccountManager.selectedAccount.supportsLocalVisibility
                                }
                                QQC2.MenuItem {
                                    icon.name: "kstars_xplanet"
                                    text: i18nc("@item:inmenu Public to the world", "Public")
                                    onTriggered: backend.visibility = Post.Public
                                }
                                QQC2.MenuItem {
                                    icon.name: "unlock"
                                    text: i18nc("@item:inmenu Public but less so", "Unlisted")
                                    onTriggered: backend.visibility = Post.Unlisted
                                }
                                QQC2.MenuItem {
                                    icon.name: "lock"
                                    text: i18nc("@item:inmenu Only to followers", "Private")
                                    onTriggered: backend.visibility = Post.Private
                                }
                                QQC2.MenuItem {
                                    icon.name: "mail-message"
                                    text: i18nc("@item:inmenu Only to who is explicitly mentioned", "Direct Message")
                                    onTriggered: backend.visibility = Post.Direct
                                }
                            }
                            QQC2.ToolTip {
                                text: i18nc("@info:tooltip Post visibility", "Visibility")
                            }
                        }

                        QQC2.ToolButton {
                            id: languageButton
                            text: backend.language
                            QQC2.ToolTip.text: i18nc("@info:tooltip", "Post language")
                            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                            QQC2.ToolTip.visible: hovered
                            checkable: true

                            onClicked: languageSelect.open()
                            LanguageSelector {
                                id: languageSelect

                                onAboutToShow: {
                                    const sourceIndex = listView.model.sourceModel.indexOfValue(backend.language);
                                    listView.currentIndex = listView.model.mapFromSource(sourceIndex).row;
                                }
                                onCodeSelected: code => backend.language = code
                            }
                        }

                        QQC2.ToolSeparator {}

                        QQC2.ToolButton {
                            id: emojiButton

                            Layout.alignment: Qt.AlignRight

                            icon.name: "smiley"
                            onClicked: emojiDialog.open()
                            EmojiDialog {
                                id: emojiDialog

                                modal: false

                                onChosen: (emoji) => textArea.insert(textArea.cursorPosition, emoji)
                                onClosed: if (emojiButton.checked) emojiButton.checked = false
                            }
                            QQC2.ToolTip.text: i18nc("@info:tooltip", "Add emoji")
                            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                            QQC2.ToolTip.visible: hovered
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Rectangle {
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                color: Kirigami.Theme.backgroundColor
                radius: Kirigami.Units.cornerRadius

                implicitWidth: characterCountLabel.implicitWidth + Kirigami.Units.smallSpacing
                implicitHeight: characterCountLabel.implicitHeight + Kirigami.Units.smallSpacing

                QQC2.Label {
                    id: characterCountLabel

                    anchors.fill: parent

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: i18nc("@label Character count in the status composer", "<b>%1/%2</b> characters", root.backend.charactersLeft, AccountManager.selectedAccount.maxPostLength)
                }
            }

            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                icon.name: {
                    switch (root.purpose) {
                        case StatusComposer.New:
                            return "document-send-symbolic";
                        case StatusComposer.Reply:
                            return "tokodon-post-reply";
                        case StatusComposer.Redraft:
                            return "edit-redo-symbolic";
                        case StatusComposer.Edit:
                            return "document-save-symbolic";
                    }
                }
                text: {
                    switch (root.purpose) {
                        case StatusComposer.New:
                            return i18nc("@action:button Send a post", "Send");
                        case StatusComposer.Reply:
                            return i18nc("@action:button Reply to a post", "Reply");
                        case StatusComposer.Redraft:
                            return i18nc("@action:button Send the same post again", "Repost");
                        case StatusComposer.Edit:
                            return i18nc("@action:Button Save an edited a post", "Save");
                    }
                }
                enabled: root.isStatusValid && root.isPollValid && (!progress.uploading || backend.attachmentEditorModel.count > 0)
                Layout.alignment: Qt.AlignRight
                onClicked: root.submitPost()
            }
        }
    }

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Image {
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            source: "qrc:/content/elephant.svg"
            asynchronous: true
        }
    }
}
