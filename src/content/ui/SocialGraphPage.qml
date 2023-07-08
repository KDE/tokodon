// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0
import org.kde.kirigamiaddons.delegates 1.0 as Delegates

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

        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property var index
            required property var identity

            text: identity.displayName

            onClicked: Navigation.openAccount(delegate.identity.id)

            contentItem: ColumnLayout {
                spacing: 0

                RowLayout {
                    Layout.fillWidth: true

                    InlineIdentityInfo {
                        identity: delegate.identity
                        secondary: false
                        avatar.actions.main: null
                    }

                    QQC2.Button {
                        text: i18nc("@action:button Allow follow request", "Allow")
                        icon.name: "checkmark"
                        onClicked: model.actionAllow(model.index(delegate.index, 0))
                        visible: model.isFollowRequest
                    }

                    QQC2.Button {
                        text: i18nc("@action:button Deny follow request", "Deny")
                        icon.name: "cards-block"
                        onClicked: model.actionDeny(model.index(delegate.index, 0))
                        visible: model.isFollowRequest
                    }
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

        QQC2.ProgressBar {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
            indeterminate: true
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: listview.model.placeholderText
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}