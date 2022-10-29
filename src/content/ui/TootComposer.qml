// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only
import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQml.Models 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import org.kde.kmasto 1.0

MastoPage {
    title: i18n("Write a new toot")
    property var postObject

    Kirigami.FlexColumn {
        maximumWidth: Kirigami.Units.gridUnit * 30
        padding: 0
        QQC2.TextField {
            placeholderText: i18n("Content Warning")
            Layout.fillWidth: true
            visible: contentWarning.checked
            onTextChanged: postObject.subject = text
        }

        QQC2.TextArea {
            id: textArea
            placeholderText: i18n("What's new?")
            text: postObject.mentions.join(" ")
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 12 + actions.implicitHeight
            bottomInset: -1
            leftInset: -1
            rightInset: -1
            onTextChanged: postObject.content = text
            Kirigami.SpellChecking.enabled: true
            Item {
                id: actions
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    leftMargin: 1
                    rightMargin: 1
                    bottomMargin: 1
                }
                implicitHeight: {
                    let h = 0;
                    for (var i = 0; i < visibleChildren.length; ++i) {
                        h += Math.ceil(visibleChildren[i].implicitHeight);
                    }
                    return h;
                }
                height: implicitHeight

                Behavior on height  {
                    NumberAnimation {
                        property: "height"
                        duration: Kirigami.Units.shortDuration
                        easing.type: Easing.OutCubic
                    }
                }

                GridLayout {
                    id: attachmentLayout
                    width: parent.width
                    height: visible ? implicitHeight : 0
                    visible: attachmentModel.count > 0
                    implicitHeight: Kirigami.Units.gridUnit * 20
                    anchors.bottom: pollSeparator.top
                    columns: repeater.count === 0 ? 1 : 2
                    Repeater {
                        id: repeater
                        model: AttachmentEditorModel {
                            id: attachmentModel
                            post: postObject
                        }

                        Image {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.maximumWidth: Kirigami.Units.gridUnit * 30
                            Layout.preferredWidth: Kirigami.Units.gridUnit * (repeater.count === 1 ? 30 : 15)
                            Layout.preferredHeight: Kirigami.Units.gridUnit * (repeater.count > 2 ? 20 : 10)
                            Layout.margins: Kirigami.Units.largeSpacing
                            fillMode: Image.PreserveAspectCrop
                            source: model.preview
                        }
                    }
                }

                Kirigami.Separator {
                    id: pollSeparator
                    visible: addPool.checked
                    width: parent.width
                    anchors.bottom: poll.top
                }

                Column {
                    id: poll
                    width: parent.width
                    anchors.bottom: actionsToolbar.top
                    Repeater {
                        model: ListModel {
                            id: pollModel
                            property bool multipleChoice: true
                            ListElement {
                                name: ""
                            }
                            ListElement {
                                name: ""
                            }
                        }

                        Kirigami.AbstractListItem {
                            background: null
                            visible: addPool.checked
                            contentItem: RowLayout {
                                QQC2.RadioButton {
                                    autoExclusive: pollModel.multipleChoice
                                    QQC2.ToolTip {
                                        text: i18n("Make pool auto-exclusive")
                                    }
                                    TapHandler {
                                        onTapped: pollModel.multipleChoice = !pollModel.multipleChoice
                                    }
                                }
                                QQC2.TextField {
                                    Layout.fillWidth: true
                                    placeholderText: i18n("Choice %1", index + 1)
                                }
                                QQC2.ToolButton {
                                    icon.name: "edit-delete-remove"
                                    enabled: pollModel.count > 2
                                }
                            }
                        }
                    }
                }

                QQC2.ToolBar {
                    id: actionsToolbar
                    width: parent.width
                    anchors.bottom: parent.bottom
                    RowLayout {
                        QQC2.ToolButton {
                            visible: !addPool.checked
                            icon.name: "mail-attachment-symbolic"
                            onClicked: fileDialog.open()
                            enabled: repeater.count < 4
                            FileDialog {
                                id: fileDialog
                                folder: shortcuts.home
                                title: i18n("Please choose a file")
                                onAccepted: postObject.uploadAttachment(fileDialog.fileUrl)
                            }
                            QQC2.ToolTip {
                                text: i18n("Attach File")
                            }
                        }
                        QQC2.ToolButton {
                            id: addPool
                            icon.name: "gnumeric-graphguru"
                            checkable: true
                            QQC2.ToolTip {
                                text: i18n("Add Poll")
                            }
                        }
                        QQC2.ToolButton {
                            icon.name: {
                                switch (postObject.visibility) {
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
                            QQC2.Menu {
                                id: visibilityMenu
                                QQC2.MenuItem {
                                    icon.name: "kstars_xplanet"
                                    text: i18n("Public")
                                    onTriggered: postObject.visibility = Post.Public
                                }
                                QQC2.MenuItem {
                                    icon.name: "unlock"
                                    text: i18n("Unlisted")
                                    onTriggered: postObject.visibility = Post.Unlisted
                                }
                                QQC2.MenuItem {
                                    icon.name: "lock"
                                    text: i18n("Private")
                                    onTriggered: postObject.visibility = Post.Private
                                }
                                QQC2.MenuItem {
                                    icon.name: "mail-message"
                                    text: i18n("Direct Message")
                                    onTriggered: postObject.visibility = Post.Direct
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
                    }
                }
            }
        }

        QQC2.Button {
            text: i18n("Send")
            enabled: textArea.text.length > 0 || attachmentModel.rowCount() > 0
            Layout.alignment: Qt.AlignRight
            onClicked: {
                AccountManager.selectedAccount.postStatus(postObject);
                applicationWindow().pageStack.layers.pop();
            }
        }
    }
}
