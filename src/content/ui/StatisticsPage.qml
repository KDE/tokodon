// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigamiaddons.formcard 1 as FormCard
import org.kde.kirigamiaddons.components 1 as Components
import org.kde.kirigamiaddons.tableview as TableView
import org.kde.kitemmodels as KItemModels

import org.kde.tokodon

Kirigami.Page {
    id: root

    title: "Metrics"

    actions: [
        Kirigami.Action {
            text: "Sync"
            icon.name: "view-refresh-symbolic"
            onTriggered: syncPrompt.open()
        },
        Kirigami.Action {
            text: "Clear Data"
        },
        Kirigami.Action {
            text: "Invalidate"
            onTriggered: rowLimitModel.invalidate()
        }
    ]

    Kirigami.PromptDialog {
        id: syncPrompt

        title: i18nc("@title", "Sync")
        subtitle: i18nc("@label", "Are you sure you want to sync? This will download your profile posts and may temporarily rate-limit your account. Please be mindful of your server and only do this rarely.")
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
        showCloseButton: false

        onAccepted: database.fill(AccountManager.selectedAccount)
    }

    PostStatisticsDatabase {
        id: database
    }

    Kirigami.LoadingPlaceholder {
        anchors.centerIn: parent
        visible: database.loading
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent

        visible: false // !database.loading
        text: "No Posts Synced"
        explanation: "Synchronize this account's public posts to compare and view metrics.\nTokodon saves public posts and information only, and the data stays on this device."
    }

    ColumnLayout {
        anchors.fill: parent

        QQC2.TabBar {
            id: bar

            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true

            QQC2.TabButton {
                text: i18nc("@item:inmenu Profile Post Filter", "Overview")
                implicitWidth: bar.width / 2
            }
            QQC2.TabButton {
                text: i18nc("@item:inmenu Profile Post Filter", "Posts")
                implicitWidth: bar.width / 2
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: bar.currentIndex
            Item {
                id: homeTab
            }
            QQC2.ScrollView {
                TableView.ListTableView {
                    id: table

                    clip: true

                    model: KItemModels.KSortFilterProxyModel
                        {
                            id: proxy
                            sourceModel: PostStatisticsModel {
                                database: database
                            }

                            filterRowCallback: function(source_row, source_parent) {
                                let index = proxy.mapFromSource(sourceModel.index(source_row, 0, source_parent));
                                console.info(index.row);
                                if (index.row < 15) {
                                    return true
                                } else {
                                    return false;
                                }
                            }
                        }

                    onColumnClicked: (column, headerComponent) => {
                        proxy.sortRoleName = headerComponent.textRole;

                        if (proxy.sortOrder === Qt.AscendingOrder) {
                            proxy.sortOrder = Qt.DescendingOrder;
                        } else {
                            proxy.sortOrder = Qt.AscendingOrder;
                        }
                    }

                    headerComponents: [
                        TableView.HeaderComponent {
                            width: 240
                            title: i18nc("@title:column", "Content")
                            textRole: "content"
                        },
                        TableView.HeaderComponent {
                            width: 240
                            title: i18nc("@title:column", "Favorites")
                            textRole: "favoriteCount"
                        },
                        TableView.HeaderComponent {
                            width: 240
                            title: i18nc("@title:column", "Boosts")
                            textRole: "boostCount"
                        },
                        TableView.HeaderComponent {
                            width: 240
                            title: i18nc("@title:column", "Date")
                            textRole: "publishedAt"
                        }
                    ]
                }
            }
        }
    }
}
