// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.tokodon
import QtQuick.Dialogs
import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import ".."

ColumnLayout {
    id: root

    required property AbstractAccount account

    readonly property ProfileEditorBackend backend : ProfileEditorBackend {
        account: root.account
        onSendNotification: message => applicationWindow().showPassiveNotification(message)
    }

    readonly property bool canEditProfile: !AccountManager.accountHasIssue(account)

    data: [
        Component {
            id: openFileDialog
            FileDialog {
                signal chosen(string path)
                title: i18n("Please choose a file")
                currentFolder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
                onAccepted: chosen(selectedFile)
            }
        }
    ]

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 4
        Layout.bottomMargin: Kirigami.Units.largeSpacing * 4

        enabled: canEditProfile

        Rectangle {
            Layout.preferredHeight: Kirigami.Units.gridUnit * 9
            Layout.fillWidth: true
            clip: true
            color: Kirigami.Theme.backgroundColor
            Kirigami.Theme.colorSet: Kirigami.Theme.View

            ProfileHeader {
                backgroundUrl: backend.backgroundUrl
                avatarUrl: backend.avatarUrl
                displayName: backend.displayNameHtml
                account: backend.account.identity.account
            }
        }

        FormCard.FormTextFieldDelegate {
            label: i18n("Display Name")
            text: backend.displayName
            onTextChanged: backend.displayName = text
        }

        FormCard.FormDelegateSeparator {}

        FormCard.AbstractFormDelegate {
            background: null
            Layout.fillWidth: true

            onClicked: bioField.clicked()
            onActiveFocusChanged: if (activeFocus) {
                bioField.forceActiveFocus();
            }

            contentItem: ColumnLayout {
                QQC2.Label {
                    text: i18n("Bio")
                    Layout.fillWidth: true
                }
                QQC2.TextArea {
                    id: bioField
                    Layout.fillWidth: true
                    wrapMode: TextEdit.Wrap
                    text: backend.note
                    onTextChanged: backend.note = text
                    activeFocusOnTab: false

                    KeyNavigation.tab: nextItemInFocusChain(true)
                    KeyNavigation.priority: KeyNavigation.BeforeItem
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.AbstractFormDelegate {
            id: headerDelegate

            background: null
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                QQC2.Label {
                    text: i18n("Header")
                    Layout.fillWidth: true
                }

                RowLayout {
                    Layout.fillWidth: true
                    QQC2.RoundButton {
                        id: headerUpload
                        icon.name: 'cloud-upload'
                        property var fileDialog: null;
                        Layout.alignment: Qt.AlignHCenter
                        onClicked: {
                            if (fileDialog !== null) {
                                return;
                            }

                            fileDialog = openFileDialog.createObject(QQC2.ApplicationWindow.Overlay)

                            fileDialog.onAccepted.connect(function() {
                                const receivedSource = headerUpload.fileDialog.selectedFile;
                                headerUpload.fileDialog = null;
                                if (!receivedSource) {
                                    return;
                                }
                                backend.backgroundUrl = receivedSource;
                            });
                            fileDialog.onRejected.connect(function() {
                                headerUpload.fileDialog = null;
                            });
                            fileDialog.open();
                        }
                    }
                    QQC2.Label {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        text: i18n("PNG, GIF or JPG. At most 2 MB. Will be downscaled to 1500x500px")
                        wrapMode: Text.WordWrap
                    }
                }
                QQC2.Label {
                    Layout.fillWidth: true
                    visible: text.length > 0
                    color: Kirigami.Theme.negativeTextColor
                    text: backend.backgroundUrlError
                    wrapMode: Text.WordWrap
                }

                Kirigami.LinkButton {
                    text: i18n("Delete")
                    color: Kirigami.Theme.negativeTextColor
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    onClicked: backend.backgroundUrl = ''
                }
            }
        }

        FormCard.FormDelegateSeparator { below: headerDelegate }

        FormCard.AbstractFormDelegate {
            background: null
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                QQC2.Label {
                    text: i18n("Avatar")
                    Layout.fillWidth: true
                }

                RowLayout {
                    QQC2.RoundButton {
                        id: avatarUpload
                        icon.name: 'cloud-upload'
                        property var fileDialog: null;
                        Layout.alignment: Qt.AlignHCenter
                        onClicked: {
                            if (fileDialog !== null) {
                                return;
                            }

                            fileDialog = openFileDialog.createObject(QQC2.ApplicationWindow.Overlay)

                            fileDialog.chosen.connect(function(receivedSource) {
                                avatarUpload.fileDialog = null;
                                if (!receivedSource) {
                                    return;
                                }
                                backend.avatarUrl = receivedSource;
                            });
                            fileDialog.onRejected.connect(function() {
                                avatarUpload.fileDialog = null;
                            });
                            fileDialog.open();
                        }
                    }
                    QQC2.Label {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        text: i18n("PNG, GIF or JPG. At most 2 MB. Will be downscaled to 400x400px")
                        wrapMode: Text.WordWrap
                    }
                }

                QQC2.Label {
                    visible: text.length > 0
                    Layout.fillWidth: true
                    text: backend.avatarUrlError
                    wrapMode: Text.WordWrap
                    color: Kirigami.Theme.negativeTextColor
                }

                Kirigami.LinkButton {
                    text: i18n("Delete")
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    color: Kirigami.Theme.negativeTextColor
                    onClicked: backend.avatarUrl = ''
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.AbstractFormDelegate {
            background: null
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: Kirigami.Units.largeSpacing

                QQC2.Label {
                    text: i18nc("Profile fields", "Fields")
                }

                Repeater {
                    model: backend.fields

                    delegate: RowLayout {
                        id: delegate

                        required property int index
                        required property var modelData

                        spacing: Kirigami.Units.largeSpacing

                        QQC2.TextField {
                            Layout.fillWidth: true
                            Layout.preferredWidth: parent.width / 2
                            text: delegate.modelData.name
                            activeFocusOnTab: false
                            onTextEdited: backend.setFieldName(delegate.index, text)
                        }

                        QQC2.TextField {
                            id: valueArea

                            Layout.fillWidth: true
                            Layout.preferredWidth: parent.width / 2
                            text: delegate.modelData.value
                            activeFocusOnTab: false
                            onEditingFinished: backend.setFieldValue(delegate.index, text)
                        }
                    }
                }

                RowLayout {
                    spacing: 0

                    QQC2.Button {
                        icon.name: "list-remove-symbolic"
                        text: i18nc("@action:button", "Remove")
                        enabled: backend.fields.length > 0
                        onClicked: backend.removeField()
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    QQC2.Button {
                        icon.name: "list-add-symbolic"
                        text: i18nc("@action:button", "Add")
                        enabled: backend.fields.length < backend.maxFields
                        onClicked: backend.addField()
                    }
                }
            }
        }
    }

    property QQC2.ToolBar profileFooter: QQC2.ToolBar {
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                text: i18n("Reset")
                icon.name: 'edit-reset'
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: backend.fetchAccountInfo()
            }

            QQC2.Button {
                text: i18n("Apply")
                icon.name: 'dialog-ok'
                enabled: backend.backgroundUrlError.length === 0 && backend.avatarUrlError.length === 0
                Layout.margins: Kirigami.Units.smallSpacing
                onClicked: backend.save()
            }
        }
    }
}
