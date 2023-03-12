// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.20 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import org.kde.kmasto 1.0
import './StatusDelegate'

ListView {
    id: root

    required property string text

    model: SearchModel {
        id: searchModel
    }

    section {
        property: "type"
        delegate: Kirigami.ListSectionHeader {
            text: searchModel.labelForType(section)
        }
    }

    Kirigami.PlaceholderMessage {
        text: i18n("No search results")
        visible: root.count === 0 && root.text.length > 2
        icon.name: "system-search"
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
    }

    Kirigami.PlaceholderMessage {
        text: i18n("Search for peoples, tags and posts")
        visible: root.count === 0 && root.text.length <= 2
        icon.name: "system-search"
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
    }

    delegate: DelegateChooser {
        role: "type"
        DelegateChoice {
            roleValue: SearchModel.Account
            QQC2.ItemDelegate {
                id: accountDelegate

                required property var authorIdentity

                width: ListView.view.width

                leftPadding: Kirigami.Units.largeSpacing
                rightPadding: Kirigami.Units.largeSpacing
                topPadding: Kirigami.Units.smallSpacing
                bottomPadding: Kirigami.Units.smallSpacing

                onClicked: Navigation.openAccount(accountDelegate.authorIdentity.id)

                contentItem: RowLayout {
                    Kirigami.Avatar {
                        Layout.alignment: Qt.AlignTop
                        Layout.rowSpan: 5
                        source: accountDelegate.authorIdentity.avatarUrl
                        cache: true
                        name: accountDelegate.authorIdentity.displayName
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.bottomMargin: Kirigami.Units.smallSpacing
                        Layout.leftMargin: Kirigami.Units.largeSpacing
                        spacing: 0
                        Kirigami.Heading {
                            id: heading
                            level: 5
                            text: accountDelegate.authorIdentity.displayNameHtml
                            textFormat: Text.RichText
                            type: Kirigami.Heading.Type.Primary
                            color: Kirigami.Theme.textColor
                            verticalAlignment: Text.AlignTop

                            Layout.fillWidth: true
                        }

                        Kirigami.Heading {
                            level: 5
                            elide: Text.ElideRight
                            color: Kirigami.Theme.disabledTextColor
                            text: `@${accountDelegate.authorIdentity.account}`
                            verticalAlignment: Text.AlignTop

                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }

        DelegateChoice {
            roleValue: SearchModel.Status
            StatusDelegate {
                width: ListView.view.width
                leftPadding: Kirigami.Units.largeSpacing
                rightPadding: Kirigami.Units.largeSpacing
                topPadding: Kirigami.Units.smallSpacing
                bottomPadding: Kirigami.Units.smallSpacing
                secondary: true
                showSeparator: true
                showInteractionButton: false
            }
        }

        DelegateChoice {
            roleValue: SearchModel.Hashtag
            QQC2.ItemDelegate {
                required property string id

                width: ListView.view.width
                leftPadding: Kirigami.Units.largeSpacing
                rightPadding: Kirigami.Units.largeSpacing
                topPadding: Kirigami.Units.smallSpacing
                bottomPadding: Kirigami.Units.smallSpacing

                onClicked: Navigation.openTag(id)

                contentItem: ColumnLayout {
                    Layout.fillWidth: true
                    Layout.bottomMargin: Kirigami.Units.smallSpacing
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    spacing: 0
                    Kirigami.Heading {
                        id: heading
                        level: 5
                        text: "#" + id
                        type: Kirigami.Heading.Type.Primary
                        color: Kirigami.Theme.textColor
                        verticalAlignment: Text.AlignTop
                    }
                }
            }
        }
    }
}
