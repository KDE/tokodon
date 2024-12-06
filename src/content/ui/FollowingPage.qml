// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts

import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.delegates 1 as Delegates
import org.kde.kitemmodels as KItemModels
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import org.kde.tokodon

import "./PostDelegate"

Kirigami.Page {
    id: root

    title: i18nc("@title 'Following' as in the users you're following on Mastodon.", "Following")

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    property string currentAccountId

    property AccountModel accountModel: AccountModel {
        accountId: root.currentAccountId

        excludeBoosts: !showBoostsAction.checked
        excludeReplies: !showRepliesAction.checked
        excludePinned: true
    }

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    actions: [
        Kirigami.Action {
            id: filterAction
            text: i18nc("@action:button", "Filters")
            icon.name: "view-filter"

            Kirigami.Action {
                id: showBoostsAction
                text: i18nc("@action:inmenu", "Show Boosts")
                icon.name: "tokodon-post-boost"
                checkable: true
                checked: true
                enabled: root.currentAccountId !== ""
            }
            Kirigami.Action {
                id: showRepliesAction
                text: i18nc("@action:inmenu", "Show Replies")
                icon.name: "tokodon-post-reply"
                checkable: true
                checked: true
                enabled: root.currentAccountId !== ""
            }
        }
    ]

    contentItem: Kirigami.ColumnView {
        id: columnView

        readonly property bool isOneColumn: root.width < Kirigami.Units.gridUnit * 50

        columnResizeMode: isOneColumn ? Kirigami.ColumnView.SingleColumn : Kirigami.ColumnView.FixedColumns

        QQC2.ScrollView {
            Layout.preferredWidth: 350
            Layout.fillHeight: true

            ListView {
                id: listview

                model: KItemModels.KSortFilterProxyModel {
                    sourceModel: SocialGraphModel {
                        id: socialGraphModel
                        name: "following"
                        accountId: AccountManager.selectedAccount.identity.id
                    }
                    sortRoleName: "lastStatusAt"
                    sortOrder: Qt.DescendingOrder
                }

                currentIndex: -1
                clip: true

                delegate: Delegates.RoundedItemDelegate {
                    id: delegate

                    required property var identity
                    required property string relativeTime

                    text: identity.username
                    highlighted: root.currentAccountId === identity.id

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.mediumSpacing

                        KirigamiComponents.Avatar {
                            id: avatar

                            Layout.alignment: Qt.AlignTop
                            Layout.rowSpan: 5

                            source: delegate.identity.avatarUrl
                            cache: true
                            name: delegate.identity.displayName
                        }

                        ColumnLayout {
                            id: layout

                            Layout.fillWidth: true

                            spacing: 0
                            clip: true

                            Kirigami.Heading {
                                level: 4
                                text: delegate.identity.displayNameHtml
                                type: Kirigami.Heading.Type.Primary
                                verticalAlignment: Text.AlignTop
                                elide: Text.ElideRight
                                textFormat: Text.StyledText
                                maximumLineCount: 1

                                Layout.fillWidth: true
                            }

                            QQC2.Label {
                                elide: Text.ElideRight
                                color: Kirigami.Theme.disabledTextColor
                                text: delegate.relativeTime
                                verticalAlignment: Text.AlignTop
                                maximumLineCount: 1

                                Layout.fillWidth: true
                            }
                        }
                    }

                    onClicked: {
                        root.currentAccountId = identity.id;
                        if (columnView.isOneColumn) {
                            pageStack.push(accountTimeline, {});
                        } else {
                            loader.item?.forceActiveFocus(Qt.MouseFocusReason);
                        }
                    }
                }
            }
        }

        Loader {
            id: loader

            active: !columnView.isOneColumn

            Layout.fillWidth: true
            Layout.fillHeight: true

            sourceComponent: QQC2.ScrollView {
                anchors.fill: parent

                focus: true
                clip: true

                QQC2.ScrollBar.vertical.interactive: false

                Keys.onPressed: event => timelineListView.handleKeyEvent(event)

                TimelineView {
                    id: timelineListView

                    model: root.accountModel

                    Kirigami.PlaceholderMessage {
                        id: missingAccountMessage

                        anchors.centerIn: parent
                        width: parent.width - Kirigami.Units.gridUnit * 4
                        visible: root.currentAccountId === ""

                        icon.name: "user-group-properties-symbolic"
                        text: i18n("No User Selected")
                        explanation: i18n("With the Following view, easily check on each user you're following and only their posts - sorted by recent activity.")
                    }
                }
            }
        }
    }

    Component {
        id: accountTimeline
        TimelinePage {
            model: root.accountModel
        }
    }
}
