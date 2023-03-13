// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0
import './StatusDelegate'

QQC2.Control {
    id: root

    property alias text: searchField.text
    property bool hasSearched: false

    leftPadding: 0
    topPadding: 0
    bottomPadding: 0
    rightPadding: 0

    property Kirigami.SearchField searchField: Kirigami.SearchField {
        id: searchField

        autoAccept: false
        selectByMouse: true
        width: root.parent.width - 6
        onWidthChanged: width = root.parent.width - 6

        onFocusChanged: {
            if (applicationWindow().wideScreen && focus) {
                popup.open()
            }
        }

        onAccepted: if (text.length > 2) {
            searchModel.search(text)
            root.hasSearched = true
        } else {
            popup.close();
        }

        property alias popup: popup
    }

    contentItem: searchField

    QQC2.Popup {
        id: popup

        x: -20
        y: -20

        rightPadding: 1
        leftPadding: 5
        topPadding: 0
        bottomPadding: Kirigami.Units.smallSpacing

        leftInset: 4

        width: root.parent.width - 4
        onWidthChanged: width = root.parent.width - 4
        height: Kirigami.Units.gridUnit * 20

        property int searchFieldWidth

        onAboutToShow: {
            searchFieldWidth = searchField.width
            searchField.parent = fieldContainer
            fieldContainer.contentItem = searchField
            searchField.background.visible = false
            playOpenHeight.running = true
            playOpenWidth.running = true
            playOpenX.running = true
            playOpenY.running = true
        }

        onAboutToHide: {
            root.contentItem = searchField;
            searchField.parent = root;
            searchField.background.visible = true
            searchField.focus = false
            searchField.width = searchFieldWidth;
            playCloseHeight.running = true
            playCloseWidth.running = true
            playCloseX.running = true
            playCloseY.running = true
        }

        background: Kirigami.ShadowedRectangle{
            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            color: Kirigami.Theme.backgroundColor
            radius: 7
            shadow.size: 20
            shadow.yOffset: 5
            shadow.color: Qt.rgba(0, 0, 0, 0.2)

            border.width: 1
            border.color: Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.3);
        }

        NumberAnimation on height{
            id: playOpenHeight
            easing.type: Easing.OutCubic
            from: 40
            duration: Kirigami.Units.longDuration
            to: Kirigami.Units.gridUnit * 20 + 40
            running: false
        }

        NumberAnimation on width {
            id: playOpenWidth
            easing.type: Easing.OutCubic
            from: searchField.width
            duration: Kirigami.Units.shortDuration
            to: searchField.width + 40
            running: false
        }

        NumberAnimation on x {
            id: playOpenX
            easing.type: Easing.OutCubic
            from: 0
            to: -20
            duration: Kirigami.Units.shortDuration
            running: false
        }

        NumberAnimation on y {
            id: playOpenY
            easing.type: Easing.OutCubic
            from: 0
            to: -5
            duration: Kirigami.Units.shortDuration
            running: false
        }

        NumberAnimation on height{
            id: playCloseHeight
            easing.type: Easing.OutCubic
            from: Kirigami.Units.gridUnit * 20 + 40
            duration: Kirigami.Units.longDuration
            to: searchField.heigth + 40
            running: false
        }

        NumberAnimation on width{
            id: playCloseWidth
            easing.type: Easing.OutCubic
            from: searchField.width + 40
            duration: Kirigami.Units.shortDuration
            to: searchField.width + 30
            running: false
        }

        NumberAnimation on x{
            id: playCloseX
            easing.type: Easing.OutCubic
            from: -20
            to: -15
            duration: Kirigami.Units.shortDuration
            running: false
        }

        NumberAnimation on y{
            id: playCloseY
            easing.type: Easing.OutCubic
            from: -5
            to: -0
            duration: Kirigami.Units.shortDuration
            running: false
        }

        contentItem: ColumnLayout {
            id: content
            spacing: 0

            QQC2.Control{
                id: fieldContainer

                topPadding: 0
                leftPadding: 0
                rightPadding: 0
                bottomPadding: 0

                Layout.fillWidth: true
                Layout.bottomMargin: 2
                Layout.topMargin: 3
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            QQC2.ScrollView {
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                Layout.fillHeight: true
                Layout.fillWidth: true

                background: Rectangle {
                    color: Kirigami.Theme.backgroundColor
                }

                ListView {
                    id: searchView
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
                        visible: searchView.count === 0 && root.hasSearched && !searchView.model.loading
                        icon.name: "system-search"
                        anchors.centerIn: parent
                        width: parent.width - Kirigami.Units.gridUnit * 4
                    }

                    Kirigami.PlaceholderMessage {
                        text: i18n("Loading...")
                        visible: searchView.count === 0 && searchView.model.loading
                        icon.name: "system-search"
                        anchors.centerIn: parent
                        width: parent.width - Kirigami.Units.gridUnit * 4
                    }

                    Kirigami.PlaceholderMessage {
                        text: i18n("Search for peoples, tags and posts")
                        visible: !root.hasSearched && !searchView.model.loading
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
            }
        }
    }
}
