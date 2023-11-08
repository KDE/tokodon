// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.kirigamiaddons.delegates 1 as Delegates
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import "../StatusDelegate"

Kirigami.ScrollablePage {
    title: i18n("Accounts Tool Page")
    id: root

    header: ColumnLayout {
        id: comboboxColumn
        spacing: 0
        Layout.fillWidth: true
        RowLayout {
            id: filterOptions
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            Layout.leftMargin: Kirigami.Units.largeSpacing * 5
            Layout.rightMargin: Kirigami.Units.largeSpacing * 5
            Layout.fillWidth: true

            ColumnLayout {
                Kirigami.Heading {
                    level: 4
                    text: i18nc("@info:Combobox to choose Report Status", "Report Status")
                    type: Kirigami.Heading.Type.Primary
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }
                QQC2.ComboBox {
                    id: reportStatusCombobox
                    Layout.fillWidth: true
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    model: [
                        {
                            display: i18nc("@info:Filter out unresolved reports", "Unresolved"),
                            value: ""
                        },
                        {
                            display: i18nc("@info:Filter out resolved reports", "Resolved"),
                            value: "resolved"
                        },
                    ]
                    textRole: "display"
                    valueRole: "value"
                    Component.onCompleted: reportStatusCombobox.currentIndex = reportStatusCombobox.indexOfValue(reportView.model.moderationStatus);
                    onCurrentIndexChanged: reportView.model.moderationStatus = model[currentIndex].value
                }
            }

            ColumnLayout {
                Kirigami.Heading {
                    level: 4
                    text: i18nc("@info:Combobox to choose the origin of report", "Report Origin")
                    type: Kirigami.Heading.Type.Primary
                    horizontalAlignment: Text.AlignHCenter

                    Layout.fillWidth: true
                }
                QQC2.ComboBox {
                    id: originCombobox
                    Layout.fillWidth: true
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    model: [
                        {
                            display: i18nc("@info:Filter out accounts with any origin", "All"),
                            value: ""
                        },
                        {
                            display: i18nc("@info:Filter out accounts with local origin", "Local"),
                            value: "local"
                        },
                        {
                            display: i18nc("@info:Filter out accounts with remote origin", "Remote"),
                            value: "remote"
                        },
                    ]
                    textRole: "display"
                    valueRole: "value"
                    Component.onCompleted: originCombobox.currentIndex = originCombobox.indexOfValue(reportView.model.origin);
                    onCurrentIndexChanged: reportView.model.origin = model[currentIndex].value
                }
            }
        }
        Kirigami.Separator {
            Layout.fillWidth: true
        }
    }

    ListView {
        id: reportView
        model: ReportToolModel{}

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property int index
            required property var reportInfo
            visible: (delegate.reportInfo) !== null

            //hide the report if we get a {} response
            Component.onCompleted: {
                if ((delegate.reportInfo) === null)
                {
                    delegate.implicitHeight = 0
                }
            }

            implicitWidth: ListView.view.width
            Layout.fillWidth: true

            onClicked: applicationWindow().pageStack.layers.push("./MainReportToolPage.qml", {
                reportInfo: delegate.reportInfo,
                index: delegate.index,
                model: reportView.model
                })

            contentItem: Kirigami.FlexColumn {
                spacing: 0

                RowLayout {
                    spacing: 0
                    Layout.fillWidth: true
                    InlineIdentityInfo {
                        identity: delegate.reportInfo.targetAccount.userLevelIdentity
                        secondary: false
                        admin: true
                        ip: delegate.reportInfo.targetAccount.ip
                    }
                    Kirigami.Heading {
                        level: 3
                        text: delegate.reportInfo.targetAccount.loginStatus
                        type: Kirigami.Heading.Type.Secondary
                        elide: Text.ElideRight
                    }
                }
                RowLayout {
                    spacing: 0
                    Layout.fillWidth: true
                    ColumnLayout {
                        spacing: 0

                        Kirigami.Heading {
                            level: 5
                            text: i18n("Reported By:")
                            type: Kirigami.Heading.Type.Primary
                            elide: Text.ElideRight
                            Layout.alignment: Qt.AlignLeft
                            Layout.leftMargin: Kirigami.Units.smallSpacing
                        }

                        RowLayout {
                            spacing: 0
                            Layout.alignment: Qt.AlignLeft
                            Layout.rightMargin: Kirigami.Units.largeSpacing * 3
                            Item {
                                Layout.preferredWidth: height
                                Layout.preferredHeight: Kirigami.Units.gridUnit * 2

                                KirigamiComponents.Avatar {
                                    id: avatar
                                    anchors.fill: parent
                                    anchors.margins: Kirigami.Units.smallSpacing
                                    source: delegate.reportInfo.filedAccount.userLevelIdentity.avatarUrl
                                    cache: true
                                    name: delegate.reportInfo.filedAccount.userLevelIdentity.displayName
                                    implicitWidth: avatar.width
                                    implicitHeight: avatar.height
                                    Layout.rightMargin: Kirigami.Units.smallSpacing * 3
                                }
                            }
                            Kirigami.Heading {
                                level: 4
                                text: delegate.reportInfo.filedAccount.userLevelIdentity.account
                                type: Kirigami.Heading.Type.Secondary
                                elide: Text.ElideRight
                                Layout.alignment: Qt.AlignRight
                                Layout.minimumWidth: Kirigami.Units.gridUnit * 4
                            }
                        }
                    }
                    ColumnLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignLeft
                        Layout.topMargin: Kirigami.Units.largeSpacing * 2
                        Kirigami.Heading {
                            level: 5
                            text: delegate.reportInfo.comment
                            type: Kirigami.Heading.Type.Secondary
                            elide: Text.ElideRight
                            maximumLineCount: 1
                            wrapMode: Text.Wrap
                            Layout.fillWidth: true
                            clip: true
                            Layout.rightMargin: Kirigami.Units.largeSpacing * 10
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Kirigami.Icon {
                                source: `comment-symbolic`
                                color: Kirigami.Theme.disabledTextColor
                                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
                            }
                            Kirigami.Heading {
                                level: 5
                                text: delegate.reportInfo.statusCount
                                type: Kirigami.Heading.Type.Secondary
                                elide: Text.ElideRight
                            }
                            Kirigami.Icon {
                                source: `camera-web-symbolic`
                                color: Kirigami.Theme.disabledTextColor
                                Layout.preferredHeight: Kirigami.Units.largeSpacing * 2
                                Layout.preferredWidth: Kirigami.Units.largeSpacing * 2
                            }
                            Kirigami.Heading {
                                level: 5
                                text: delegate.reportInfo.mediaAttachmentCount
                                type: Kirigami.Heading.Type.Secondary
                                elide: Text.ElideRight
                            }
                        }
                    }
                    ColumnLayout {
                        spacing: 0

                        Kirigami.Heading {
                            level: 5
                            text: i18n("Assigned Account:")
                            type: Kirigami.Heading.Type.Primary
                            elide: Text.ElideRight
                            Layout.alignment: Qt.AlignLeft
                        }
                        RowLayout {
                            spacing: 0
                            Layout.alignment: Qt.AlignRight
                            Layout.fillWidth: true
                            Item {
                                Layout.preferredWidth: height
                                Layout.preferredHeight: Kirigami.Units.gridUnit * 2

                                KirigamiComponents.Avatar {
                                    anchors.fill: parent
                                    visible: delegate.reportInfo.assignedModerator
                                    anchors.margins: Kirigami.Units.smallSpacing
                                    source: delegate.reportInfo.assignedModerator ?  delegate.reportInfo.assignedAccount.userLevelIdentity.avatarUrl : ''
                                    name: delegate.reportInfo.assignedModerator ? delegate.reportInfo.assignedAccount.userLevelIdentity.displayName : ''
                                    implicitWidth: avatar.width
                                    implicitHeight: avatar.height
                                }
                            }
                            Kirigami.Heading {
                                level: 4
                                text: delegate.reportInfo.assignedModerator ? delegate.reportInfo.assignedAccount.userLevelIdentity.account : i18nc("@info: No account assigned to the report","N/A")
                                type: Kirigami.Heading.Type.Secondary
                                elide: Text.ElideRight
                                Layout.alignment: Qt.AlignRight
                            }
                        }
                    }
                }
                Kirigami.Separator {
                    Layout.fillWidth: true
                }

                QQC2.ProgressBar {
                    visible: reportView.model.loading && (index == reportView.count - 1)
                    indeterminate: true
                    padding: Kirigami.Units.largeSpacing * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.topMargin: Kirigami.Units.largeSpacing
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                }
            }
        }
        QQC2.ProgressBar {
            visible: reportView.model.loading && reportView.count === 0
            anchors.centerIn: parent
            indeterminate: true
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No reports found")
            visible: reportView.count === 0 && !reportView.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}


