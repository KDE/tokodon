// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kmasto 1.0
import './StatusDelegate'

Kirigami.SearchField {
    id: searchField
    autoAccept: false
    selectByMouse: true
    onPressed: if (text.length > 2 && searchView.count > 0) {
        popup.open();
    }
    onAccepted: if (text.length > 2) {
        searchModel.search(text)
        popup.open()
    } else {
        popup.close();
    }
    property alias popup: popup

    QQC2.Popup {
        padding: 1
        id: popup
        x: searchField.y
        y: searchField.y + searchField.height
        z: drawer.z + 1
        width: searchField.width
        height: Kirigami.Units.gridUnit * 20

        property int originalZ;
        onOpened: {
            originalZ = QQC2.Overlay.overlay.z;
            QQC2.Overlay.overlay.z = drawer.z + 1;
        }
        onClosed: QQC2.Overlay.overlay.z = originalZ

        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false

        contentItem: QQC2.ScrollView {
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
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

                delegate: DelegateChooser {
                    role: "type"
                    DelegateChoice {
                        roleValue: SearchModel.Account
                        QQC2.ItemDelegate {
                            width: ListView.view.width
                            leftPadding: Kirigami.Units.largeSpacing
                            rightPadding: Kirigami.Units.largeSpacing
                            topPadding: Kirigami.Units.smallSpacing
                            bottomPadding: Kirigami.Units.smallSpacing
                            onClicked: if (!pageStack.currentItem.model.accountId || model.authorId !== pageStack.currentItem.accountId) {
                                pageStack.push("qrc:/content/ui/AccountInfo.qml", {
                                    accountId: model.authorId,
                                });
                            }
                            contentItem: RowLayout {
                                Kirigami.Avatar {
                                    Layout.alignment: Qt.AlignTop
                                    Layout.rowSpan: 5
                                    source: model.avatar
                                    cache: true
                                    name: model.authorDisplayName
                                }
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    Layout.bottomMargin: Kirigami.Units.smallSpacing
                                    Layout.leftMargin: Kirigami.Units.largeSpacing
                                    spacing: 0
                                    Kirigami.Heading {
                                        id: heading
                                        level: 5
                                        text: model.authorDisplayName
                                        type: Kirigami.Heading.Type.Primary
                                        color: Kirigami.Theme.textColor
                                        verticalAlignment: Text.AlignTop
                                    }
                                    Kirigami.Heading {
                                        level: 5
                                        Layout.fillWidth: true
                                        elide: Text.ElideRight
                                        color: Kirigami.Theme.disabledTextColor
                                        text: `@${model.authorUri}`
                                        verticalAlignment: Text.AlignTop
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
                            showSeparator: false
                            showInteractionButton: false
                        }
                    }
                }
            }
        }
    }
}
