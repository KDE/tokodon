// SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.tokodon
import org.kde.kirigamiaddons.delegates 1 as Delegates
import org.kde.kirigamiaddons.components 1 as KirigamiComponents

import "../PostDelegate"

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

        currentIndex: -1
        model: ReportToolModel{}

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property int index
            required property var reportInfo
            visible: delegate.reportInfo !== null

            //hide the report if we get a {} response
            Component.onCompleted: if (!delegate.reportInfo) {
                delegate.implicitHeight = 0;
            }

            onClicked: root.QQC2.ApplicationWindow.window.pageStack.layers.push(Qt.createComponent("org.kde.tokodon", "MainReportToolPage"), {
                reportInfo: delegate.reportInfo,
                index: delegate.index,
                model: reportView.model
            })

            contentItem: Kirigami.FlexColumn {
                spacing: Kirigami.Units.smallSpacing

                RowLayout {
                    spacing: 0

                    Layout.fillWidth: true

                    InlineIdentityInfo {
                        identity: delegate.reportInfo.targetAccount.userLevelIdentity
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
                    spacing: Kirigami.Units.largeSpacing

                    Layout.fillWidth: true

                    ColumnLayout {
                        spacing: 0

                        Layout.fillWidth: true

                        Kirigami.Heading {
                            level: 5
                            text: i18n("Reported By:")
                            type: Kirigami.Heading.Type.Primary
                            elide: Text.ElideRight

                            Layout.fillWidth: true
                        }

                        RowLayout {
                            spacing: Kirigami.Units.smallSpacing

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            KirigamiComponents.Avatar {
                                id: avatar

                                name: delegate.reportInfo.filedAccount.userLevelIdentity.displayName
                                source: delegate.reportInfo.filedAccount.userLevelIdentity.avatarUrl

                                cache: true

                                Layout.preferredHeight: Kirigami.Units.iconSizes.small
                                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                            }

                            Kirigami.Heading {
                                level: 4
                                text: `@${delegate.reportInfo.filedAccount.userLevelIdentity.account}`
                                type: Kirigami.Heading.Type.Secondary
                                elide: Text.ElideRight
                                Layout.alignment: Qt.AlignRight
                                Layout.minimumWidth: Kirigami.Units.gridUnit * 4
                            }
                        }
                    }

                    ColumnLayout {
                        spacing: 0

                        Layout.fillWidth: true

                        Kirigami.Heading {
                            level: 5
                            text: i18n("Comment:")
                            type: Kirigami.Heading.Type.Primary
                            elide: Text.ElideRight

                            Layout.fillWidth: true
                        }

                        QQC2.Label {
                            text: delegate.reportInfo.comment
                            elide: Text.ElideRight
                            maximumLineCount: 1
                            wrapMode: Text.Wrap

                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }

                    ColumnLayout {
                        spacing: 0

                        Kirigami.Heading {
                            level: 5
                            text: i18n("Assigned Account:")
                            type: Kirigami.Heading.Type.Primary
                            elide: Text.ElideRight

                            Layout.fillWidth: true
                        }

                        RowLayout {
                            spacing: Kirigami.Units.smallSpacing

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            KirigamiComponents.Avatar {
                                visible: delegate.reportInfo.assignedModerator

                                source: delegate.reportInfo.assignedModerator ?  delegate.reportInfo.assignedAccount.userLevelIdentity.avatarUrl : ''
                                name: delegate.reportInfo.assignedModerator ? delegate.reportInfo.assignedAccount.userLevelIdentity.displayName : ''

                                Layout.preferredHeight: Kirigami.Units.iconSizes.small
                                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                            }

                            Kirigami.Heading {
                                level: 4
                                text: delegate.reportInfo.assignedModerator ? `@${delegate.reportInfo.assignedAccount.userLevelIdentity.account}` : i18nc("@info: No account assigned to the report","N/A")
                                elide: Text.ElideRight
                                type: Kirigami.Heading.Type.Secondary

                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }
        }

        footer: Kirigami.FlexColumn {
            width: parent.width
            maximumWidth: Kirigami.Units.gridUnit * 40
            implicitHeight: Kirigami.Units.gridUnit * 4

            spacing: Kirigami.Units.largeSpacing
            padding: 0

            visible: ListView.view.count > 0

            Kirigami.Separator {
                Layout.fillWidth: true
                visible: loadingBar.visible
            }

            QQC2.ProgressBar {
                id: loadingBar

                visible: reportView.model.loading
                indeterminate: true
                Layout.alignment: Qt.AlignHCenter
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No reports found")
            visible: reportView.count === 0 && !reportView.model.loading
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}


