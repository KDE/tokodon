// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.delegates 1 as Delegates
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import org.kde.tokodon
import './StatusDelegate'

ListView {
    id: root

    required property string text

    onTextChanged: if (text.length === 0) {
        searchModel.clear();
    }

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
        text: i18n("Loading...")
        visible: searchModel.loading
        icon.name: "system-search"
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
    }

    Kirigami.PlaceholderMessage {
        text: i18n("No search results")
        visible: root.count === 0 && root.text.length > 2 && !searchModel.loading && searchModel.loaded
        icon.name: "system-search"
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
    }

    Kirigami.PlaceholderMessage {
        text: i18n("Search for peoples, tags and posts")
        visible: root.count === 0 && !searchModel.loading && !searchModel.loaded
        icon.name: "system-search"
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
    }

    delegate: DelegateChooser {
        role: "type"
        DelegateChoice {
            roleValue: SearchModel.Account

            Delegates.RoundedItemDelegate {
                id: accountDelegate

                required property var authorIdentity

                width: ListView.view.width
                text: accountDelegate.authorIdentity.displayName

                onClicked: Navigation.openAccount(accountDelegate.authorIdentity.id)

                contentItem: RowLayout {
                    KirigamiComponents.Avatar {
                        Layout.alignment: Qt.AlignTop
                        Layout.rowSpan: 5
                        source: accountDelegate.authorIdentity.avatarUrl
                        cache: true
                        name: accountDelegate.authorIdentity.displayName
                    }

                    ColumnLayout {
                        spacing: 0

                        Layout.fillWidth: true
                        Layout.bottomMargin: Kirigami.Units.smallSpacing
                        Layout.leftMargin: Kirigami.Units.largeSpacing

                        Kirigami.Heading {
                            id: heading

                            level: 5
                            text: accountDelegate.text
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
                secondary: true
                showSeparator: true
                showInteractionButton: false

                leftPadding: 0
                rightPadding: 0
                topPadding: Kirigami.Units.smallSpacing
                bottomPadding: Kirigami.Units.smallSpacing
            }
        }

        DelegateChoice {
            roleValue: SearchModel.Hashtag

            Delegates.RoundedItemDelegate {
                id: delegate

                required property string id

                text: "#" + id
                Accessible.description: i18n("Hashtag")

                onClicked: Navigation.openTag(id)

                contentItem: ColumnLayout {
                    Layout.fillWidth: true
                    Layout.bottomMargin: Kirigami.Units.smallSpacing
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    spacing: 0
                    Kirigami.Heading {
                        id: heading
                        level: 5
                        text: delegate.text
                        type: Kirigami.Heading.Type.Primary
                        color: Kirigami.Theme.textColor
                        verticalAlignment: Text.AlignTop
                    }
                }
            }
        }
    }
}
