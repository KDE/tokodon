// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

import "./StatusDelegate"

Kirigami.ScrollablePage {
    id: root
    property alias model: listview.model

    title: model.displayName
    titleDelegate: Kirigami.Heading {
        // identical to normal Kirigami headers
        Layout.fillWidth: true
        Layout.maximumWidth: implicitWidth + 1
        Layout.minimumWidth: 0
        maximumLineCount: 1
        elide: Text.ElideRight
        text: root.title
        textFormat: TextEdit.RichText
    }

    ListView {
        id: listview
        model: root.model

        delegate: QQC2.ItemDelegate {
            id: delegate

            required property var index
            required property var identity

            width: ListView.view.width

            onClicked: Navigation.openAccount(delegate.identity.id)

            contentItem: Kirigami.FlexColumn {
                spacing: 0

                maximumWidth: Kirigami.Units.gridUnit * 40

                RowLayout {
                    Layout.fillWidth: true
                    InlineIdentityInfo {
                        identity: delegate.identity
                        secondary: false
                    }

                    QQC2.Button {
                        text: "Allow"
                        icon.name: "checkmark"
                        onClicked: model.actionAllow(model.index(delegate.index, 0))
                        visible: model.isFollowRequest
                    }

                    QQC2.Button {
                        text: "Deny"
                        icon.name: "cards-block"
                        onClicked: model.actionDeny(model.index(delegate.index, 0))
                        visible: model.isFollowRequest
                    }
                }

                Kirigami.Separator {
                    visible: index !== listview.count - 1
                    Layout.fillWidth: true
                }

                QQC2.ProgressBar {
                    visible: listview.model.loading && (index == listview.count - 1)
                    indeterminate: true
                    padding: Kirigami.Units.largeSpacing * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                }
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("Loading...")
            visible: listview.count === 0 && listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
        
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: { 
                if (listview.model.isFollowRequest) {
                    return i18n("No follow requests")
                } else if (listview.model.isFollowers) {
                    return i18n("No followers")
                } else if (listview.model.isFollowing) {
                    return i18n("Not following anyone")
                } else {
                    return ""
                }
            }
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}