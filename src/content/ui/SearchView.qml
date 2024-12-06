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
import './PostDelegate'

ListView {
    id: root

    required property string text

    signal itemSelected

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
            width: parent.width
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
        text: i18n("Search for users, tags and posts")
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

                onClicked: {
                    Navigation.openAccount(accountDelegate.authorIdentity.id);
                    root.itemSelected();
                }

                contentItem: InlineIdentityInfo {
                    identity: accountDelegate.authorIdentity
                    secondary: false
                }
            }
        }

        DelegateChoice {
            roleValue: SearchModel.Status
            Delegates.RoundedItemDelegate {
                id: accountDelegate

                required property var post

                width: ListView.view.width

                onClicked: {
                    Navigation.openPost(post.postId);
                    root.itemSelected();
                }

                contentItem: ColumnLayout {
                    spacing: Kirigami.Units.smallSpacing

                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        KirigamiComponents.Avatar {
                            id: avatar

                            implicitHeight: Math.round(Kirigami.Units.gridUnit * 1.5)
                            implicitWidth: implicitHeight
                            source: post.authorIdentity.avatarUrl
                            cache: true
                            name: post.authorIdentity.displayName
                        }

                        Kirigami.Heading {
                            level: 4
                            text: post.authorIdentity.displayNameHtml
                            type: Kirigami.Heading.Type.Primary
                            verticalAlignment: Text.AlignTop
                            elide: Text.ElideRight
                            textFormat: Text.StyledText
                            maximumLineCount: 1

                            Layout.fillWidth: true
                        }
                    }

                    Loader {
                        active: post.content.length > 0
                        visible: active

                        Layout.fillWidth: true

                        sourceComponent: PostContent {
                            content: post.content
                            expandedPost: false
                            secondary: true
                            shouldOpenInternalLinks: false
                            shouldOpenAnyLinks: false
                            hoverEnabled: false

                            onClicked: accountDelegate.clicked()
                        }
                    }
                }
            }
        }

        DelegateChoice {
            roleValue: SearchModel.Hashtag

            Delegates.RoundedItemDelegate {
                id: delegate

                required property string id

                text: "#" + id
                Accessible.description: i18n("Hashtag")

                onClicked: {
                    Navigation.openTag(id);
                    root.itemSelected();
                }

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
