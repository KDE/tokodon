// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami 2 as Kirigami
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigamiaddons.components 1 as KirigamiComponents
import org.kde.tokodon

// The visual "link preview box" when there's some data attached to a link
// Such as the website page description and title
Item {
    id: root

    required property var card
    required property bool selected

    readonly property bool hasAuthorship: card.authorName !== "" || card.authorIdentity != null

    implicitWidth: mainLayout.implicitWidth
    implicitHeight: mainLayout.implicitHeight

    Accessible.name: i18n("Link preview: %1", root.card ? root.card.title : '')
    Accessible.description: root.card ? root.card.providerName : ''

    ColumnLayout {
        id: mainLayout

        anchors.fill: parent

        spacing: 0

        QQC2.Control {
            id: topControl

            activeFocusOnTab: true
            hoverEnabled: true
            padding: Kirigami.Units.largeSpacing

            function openLink(): void {
                Qt.openUrlExternally(root.card.url);
            }

            Accessible.onPressAction: openLink()
            Keys.onSpacePressed: openLink()

            TapHandler {
                onTapped: topControl.openLink()
            }

            onHoveredChanged: if (hovered) {
                applicationWindow().hoverLinkIndicator.text = root.card.url;
            } else {
                applicationWindow().hoverLinkIndicator.text = "";
            }

            Layout.fillWidth: true
            Layout.fillHeight: true

            background: Kirigami.ShadowedRectangle {
                Kirigami.Theme.colorSet: Kirigami.Theme.Window
                Kirigami.Theme.inherit: false

                color: Kirigami.Theme.alternateBackgroundColor
                corners {
                    topLeftRadius: Kirigami.Units.cornerRadius
                    topRightRadius: Kirigami.Units.cornerRadius
                    bottomLeftRadius: root.hasAuthorship ? 0 : Kirigami.Units.cornerRadius
                    bottomRightRadius: root.hasAuthorship ? 0 : Kirigami.Units.cornerRadius
                }
                border {
                    width: topControl.visualFocus || topControl.hovered ? 2 : 0
                    color: topControl.visualFocus || topControl.hovered ? Kirigami.Theme.focusColor : 'transparent'
                }
            }

            contentItem: RowLayout {
                spacing: 0

                Image {
                    id: img

                    visible: root.card && root.card.image

                    Layout.minimumHeight: Kirigami.Units.gridUnit * 3
                    Layout.maximumHeight: Kirigami.Units.gridUnit * 3
                    Layout.minimumWidth: Kirigami.Units.gridUnit * 3
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 3
                    Layout.topMargin: 0
                    Layout.bottomMargin: 0
                    Layout.leftMargin: Kirigami.Units.smallSpacing

                    layer.enabled: true
                    layer.effect: RoundedEffect {}

                    fillMode: Image.PreserveAspectCrop
                    source: root.card ? root.card.image : ''
                }
                ColumnLayout {
                    spacing: 0

                    Layout.fillWidth: true
                    Layout.margins: Kirigami.Units.smallSpacing
                    Layout.leftMargin: Kirigami.Units.largeSpacing

                    QQC2.Label {
                        text: root.card ? root.card.providerName : ''
                        elide: Text.ElideRight
                        font: Kirigami.Theme.smallFont
                        visible: text
                        maximumLineCount: 1
                        color: Kirigami.Theme.disabledTextColor

                        Layout.fillWidth: true
                    }
                    Kirigami.Heading {
                        level: 5
                        text: root.card ? root.card.title : ''
                        elide: Text.ElideRight
                        font.weight: Font.DemiBold
                        maximumLineCount: 1
                        visible: text

                        Layout.fillWidth: true
                    }
                    QQC2.Label {
                        text: root.card ? root.card.description : ''
                        elide: Text.ElideRight
                        visible: text
                        maximumLineCount: 1

                        Layout.fillWidth: true
                    }
                }
            }
        }

        QQC2.Control {
            id: authorControl

            readonly property bool hasMastodonAccount: root.card.authorIdentity != null

            activeFocusOnTab: true
            hoverEnabled: true
            visible: root.hasAuthorship
            padding: Kirigami.Units.largeSpacing

            Layout.fillWidth: true
            Layout.preferredHeight: visible ? 40 : 0

            function openLink(): void {
                if (authorControl.hasMastodonAccount) {
                    Navigation.openAccount(root.card.authorIdentity.id);
                } else {
                    Qt.openUrlExternally(root.card.authorUrl);
                }
            }

            onHoveredChanged: if (hovered && !hasMastodonAccount) {
                applicationWindow().hoverLinkIndicator.text = root.card.authorUrl;
            } else {
                applicationWindow().hoverLinkIndicator.text = "";
            }

            Accessible.onPressAction: openLink()
            Keys.onSpacePressed: openLink()

            TapHandler {
                onTapped: authorControl.openLink()
            }

            background: Kirigami.ShadowedRectangle {
                Kirigami.Theme.colorSet: Kirigami.Theme.Window
                Kirigami.Theme.inherit: false

                color: Qt.darker(Kirigami.Theme.backgroundColor, 1.1)
                corners {
                    bottomLeftRadius: Kirigami.Units.cornerRadius
                    bottomRightRadius: Kirigami.Units.cornerRadius
                }
                border {
                    width: authorControl.visualFocus || authorControl.hovered ? 2 : 0
                    color: authorControl.visualFocus || authorControl.hovered ? Kirigami.Theme.focusColor : 'transparent'
                }
            }

            contentItem: Item {
                id: authorLayout

                Loader {
                    anchors.fill: parent

                    sourceComponent: authorControl.hasMastodonAccount ? mastodonAuthor : webAuthor

                    property Component mastodonAuthor: Component {
                        RowLayout {
                            spacing: Kirigami.Units.mediumSpacing

                            QQC2.Label {
                                text: root.card ? i18nc("@info:label 'More from [this author]', where we put the user's avatar and name next to this label", "More from") : ''
                                elide: Text.ElideRight
                                verticalAlignment: Qt.AlignVCenter
                                visible: text
                                maximumLineCount: 1

                                Layout.fillHeight: true
                                Layout.leftMargin: Kirigami.Units.largeSpacing
                            }
                            KirigamiComponents.Avatar {
                                implicitHeight: Math.round(Kirigami.Units.gridUnit * 1.5)
                                implicitWidth: implicitHeight

                                name: root.card.authorIdentity ? root.card.authorIdentity.displayName : ''
                                source: root.card.authorIdentity ? root.card.authorIdentity.avatarUrl : ''
                                cache: true
                            }
                            QQC2.Label {
                                text: root.card ? root.card.authorIdentity.displayNameHtml : ''
                                elide: Text.ElideRight
                                verticalAlignment: Qt.AlignVCenter
                                visible: text
                                maximumLineCount: 1

                                Layout.fillHeight: true
                            }
                            Item {
                                Layout.fillWidth: true
                            }
                        }
                    }

                    property Component webAuthor: Component {
                        RowLayout {
                            spacing: 0

                            QQC2.Label {
                                text: root.card ? i18nc("@info:label 'This is written by %1'", "By %1", root.card.authorName) : ''
                                elide: Text.ElideRight
                                verticalAlignment: Qt.AlignVCenter
                                visible: text
                                maximumLineCount: 1

                                Layout.fillHeight: true
                                Layout.leftMargin: Kirigami.Units.largeSpacing
                            }
                        }
                    }
                }
            }
        }
    }
}
