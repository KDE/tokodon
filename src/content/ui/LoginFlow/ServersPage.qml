// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kitemmodels as KItemModels

import org.kde.tokodon

Kirigami.ScrollablePage {
    id: root

    title: i18nc("@title:window", "Pick a Server")

    property alias filterString: searchField.text
    property var account

    data: Connections {
        target: Controller
        function onNetworkErrorOccurred(error: string): void {
            errorBanner.text = i18nc("@info:status Network status", "Failed to contact server: %1. Please check your proxy settings.", error)
            errorBanner.visible = true;
        }
    }

    function handleRegistration(): void {
        if (!account.registrationsOpen) {
            if (account.registrationMessage.length > 0) {
                errorBanner.text = i18n("This server is closed for registration: %1", account.registrationMessage);
            } else {
                errorBanner.text = i18n("This server is closed for registration, and did not provide a reason.");
            }
            errorBanner.visible = true;
            return;
        }

        account.fetchedInstanceMetadata.disconnect(handleRegistration);

        Window.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "RulesPage"), {
            account: account,
        });
    }

    function register(domain: string): void {
        errorBanner.visible = false;
        account = AccountManager.createNewAccount(domain);
        account.fetchedInstanceMetadata.connect(handleRegistration);
        account.registerTokodon(false, false);
    }

    header: QQC2.Control {
        topPadding: 0
        bottomPadding: 0
        leftPadding: 0
        rightPadding: 0

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

            Kirigami.InlineMessage {
                id: errorBanner
                type: Kirigami.MessageType.Error
                width: parent.width
                position: Kirigami.InlineMessage.Position.Header

                showCloseButton: true

                Layout.fillWidth: true

                actions: Kirigami.Action {
                    text: i18n("Proxy Settings")
                    icon.name: "settings-configure"
                    onTriggered: pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "NetworkProxySettings"))
                }
            }

            Kirigami.SearchField {
                id: searchField
                placeholderText: i18n("Search or enter URL…")

                Layout.fillWidth: true
                Layout.margins: Kirigami.Units.largeSpacing

                KeyNavigation.tab: listView
            }
        }
    }

    // For some reason setting the focus property on the ListView doesn't work, and it doesn't announce items to the screen reader
    Component.onCompleted: listView.forceActiveFocus()

    ListView {
        id: listView

        model: KItemModels.KSortFilterProxyModel {
            id: filterModel

            sourceModel: PublicServersModel {
                id: publicServersModel
            }

            filterString: root.filterString
            filterRoleName: "domain"
        }

        header: Delegates.RoundedItemDelegate {
            onClicked: root.register(root.filterString)

            property int index: 0

            activeFocusOnTab: false // We handle moving to this item via up/down arrows, otherwise the tab order is wacky

            text: root.filterString
            icon.name: "compass-symbolic"

            contentItem: Delegates.SubtitleContentItem {
                itemDelegate: parent
                subtitle: i18n("Custom Server")
            }

            height: visible ? implicitHeight : 0
            visible: !publicServersModel.loading && listView.count === 0 && root.filterString !== ""
        }

        section.delegate: Kirigami.ListSectionHeader {
            width: ListView.view.width
            text: i18n("Public Servers")
        }
        section.property: "isPublic"

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property int index
            required property string domain
            required property string description
            required property string iconSource

            text: domain
            icon.source: iconSource

            contentItem: Delegates.SubtitleContentItem {
                itemDelegate: delegate
                subtitle: delegate.description
            }

            onClicked: root.register(delegate.domain)
        }

        Kirigami.LoadingPlaceholder {
            anchors.centerIn: parent
            visible: publicServersModel.loading
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No Public Servers")
            explanation: i18n("Enter a server URL manually in the search field.")
            visible: !publicServersModel.loading && listView.count === 0 && root.filterString === ""
            width: parent.width - Kirigami.Units.gridUnit * 4
        }

        KeyNavigation.backtab: searchField
    }
}
