// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.tokodon

Kirigami.ScrollablePage {
    id: root

    property string collectionId
    required property string name
    property Component editCollectionPage: Qt.createComponent("org.kde.tokodon", "EditCollectionPage", Qt.Asynchronous)

    title: name

    header: QQC2.Control {
        topPadding: Kirigami.Units.largeSpacing
        bottomPadding: Kirigami.Units.largeSpacing
        leftPadding: Kirigami.Units.largeSpacing
        rightPadding: Kirigami.Units.largeSpacing

        width: root.width

        background: Rectangle {
            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            color: Kirigami.Theme.backgroundColor

            Kirigami.Separator {
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                    right: parent.right
                }
            }
        }

        contentItem: ColumnLayout {
            spacing: 0

            Kirigami.Heading {
                text: collectionModel.name
                wrapMode: Text.WordWrap

                Layout.fillWidth: true
            }

            QQC2.Label {
                text: collectionModel.description
                wrapMode: Text.WordWrap

                Layout.fillWidth: true
            }
        }
    }

    actions: Kirigami.Action {
        text: i18nc("@action:button Edit current collection", "Edit…")
        icon.name: "edit-rename"
        visible: collectionModel.accountId === AccountManager.selectedAccount.identity.id

        onTriggered: {
            let page = pageStack.layers.push(editCollectionPage.createObject(root), {
                purpose: EditCollectionPage.Edit,
                collectionId: root.collectionId
            });
            page.done.connect(function(deleted) {
                pageStack.layers.clear();
                while (pageStack.depth > 1) {
                    pageStack.pop();
                }

                // If we end up on a collections page, make sure to reload the model as we just deleted something.
                const collectionsPage = (pageStack.currentItem as CollectionsPage);
                if (collectionsPage) {
                    collectionsPage.reload()
                } else {
                    // If we are here because someone has favorited, then we need to go back home.
                    applicationWindow().homeAction.trigger();
                }
            });
        }
    }

    ListView {
        id: listview

        currentIndex: -1

        model: CollectionModel {
            id: collectionModel

            collectionId: root.collectionId
        }

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property var identity
            required property var index

            text: identity.displayName

            onClicked: Navigation.openAccount(delegate.identity.id)

            contentItem: ColumnLayout {
                spacing: 0

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Layout.fillWidth: true

                    InlineIdentityInfo {
                        identity: delegate.identity
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    QQC2.Button {
                        text: i18nc("@action:button Remove from collection", "Remove")
                        icon.name: "list-remove-user"
                        visible: collectionModel.accountId === AccountManager.selectedAccount.identity.id

                        onClicked: collectionModel.removeItem(collectionModel.index(delegate.index, 0))
                    }
                }
            }
        }

        Kirigami.LoadingPlaceholder {
            visible: listview.model.loading && listview.count === 0
            anchors.centerIn: parent
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18nc("@info:placeholder No accounts in this collection", "No Accounts")
            visible: listview.count === 0 && !listview.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
