// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kmasto 1.0

import "./StatusDelegate"

Kirigami.ScrollablePage {
    title: i18n("Follow Requests")

    ListView {
        id: listview
        model: FollowRequestModel {}

        delegate: QQC2.ItemDelegate {
            id: delegate

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
                    }

                    QQC2.Button {
                        text: "Deny"
                        icon.name: "cards-block"
                    }
                }

                Kirigami.Separator {
                    Layout.fillWidth: true
                }
            }
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        text: i18n("Loading...")
        visible: listview.count === 0 && listview.model.loading
    }
    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        text: i18n("No Follow Requests")
        visible: listview.count === 0 && !listview.model.loading
    }
}