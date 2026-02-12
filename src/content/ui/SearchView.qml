// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import Qt.labs.qmlmodels
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.components as KirigamiComponents
import org.kde.tokodon
import './PostDelegate'

ListView {
    id: root

    clip: true

    required property string text
    property bool isPopup: !applicationWindow().checkIfCurrentPage("search")

    signal itemSelected

    header: QQC2.Control {
        padding: Kirigami.Units.smallSpacing
        width: parent.width
        height: isPopup ? implicitHeight : 0
        visible: isPopup && root.count > 0

        background: null
        contentItem: QQC2.Button {
            icon.name: "open-link-symbolic"
            text: i18nc("@action:button Pop out search results into its own page", "Pop Out")

            Accessible.description: i18nc("@info", "Pop out search results to a full page")

            onClicked: Navigation.searchFor(root.text)
        }
    }

    onTextChanged: {
        if (text.length === 0) {
            searchModel.clear();
        }
    }

    model: SearchModel {
        id: searchModel
    }

    section {
        property: "type"
        delegate: Kirigami.ListSectionHeader {
            icon.name: searchModel.iconForType(section)
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
                clip: true

                required property var authorIdentity

                text: accountDelegate.authorIdentity.displayName

                onClicked: {
                    Navigation.openAccount(accountDelegate.authorIdentity.id);
                    root.itemSelected();
                }

                contentItem: InlineIdentityInfo {
                    identity: accountDelegate.authorIdentity
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

                text: "#" + delegate.id
                onClicked: {
                    Navigation.openTag(id);
                    root.itemSelected();
                }
            }
        }
    }
}
