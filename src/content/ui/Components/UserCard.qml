import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kmasto 1.0
import '..'
import '../StatusDelegate'

Kirigami.AbstractCard {
    id: root

    required property var userIdentity

    property bool isBackgroundAvailable: !userIdentity.backgroundUrl.toString().trim().endsWith("/headers/original/missing.png")

    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }

    leftPadding: 0
    rightPadding: 0
    topPadding: 0

    showClickFeedback: true

    header: Image {
        id: bannerImage
        visible: isBackgroundAvailable
        anchors.fill: parent
        cache: true
        fillMode: Image.PreserveAspectCrop

        source: isBackgroundAvailable ? userIdentity.backgroundUrl : ''

        Layout.fillWidth: true
        Layout.preferredHeight: isBackgroundAvailable ? Kirigami.Units.gridUnit * 10 : 0


        Kirigami.ShadowedRectangle {
            id: shadowedRectangle
            anchors {
                left: parent.left
                right: parent.right
            }
            height: parent.height

            opacity: 0.5
            color: "black"
            readonly property real radiusFromBackground: root.background.radius - root.background.border.width

            corners {
                topLeftRadius: radiusFromBackground
                topRightRadius: radiusFromBackground
                bottomLeftRadius: radiusFromBackground
                bottomRightRadius: radiusFromBackground
            }
        }

    }

    contentItem: Item {
        implicitHeight: userInfo.implicitHeight
        anchors {
            leftMargin: Kirigami.Units.mediumSpacing
            rightMargin: Kirigami.Units.mediumSpacing
            topMargin: Kirigami.Units.mediumSpacing
        }

        ColumnLayout {
            id: userInfo
            InlineIdentityInfo {
                secondary: false
                identity: root.userIdentity
                Layout.bottomMargin: Kirigami.Units.smallSpacing
            }

            QQC2.TextArea {
                id: bio
                visible: root.userIdentity.bio && root.userIdentity.bio.length > 0
                font: Config.defaultFont
                width: root.width - Kirigami.Units.largeSpacing * 2
                text:  "<style>
                            a {
                            color: " + Kirigami.Theme.linkColor + ";
                            text-decoration: none;
                            }
                            </style>" + root.userIdentity.bio
                textFormat: TextEdit.RichText
                readOnly: true
                background: null
                wrapMode: Text.Wrap
                selectByMouse: !Kirigami.Settings.isMobile
                onLinkActivated: {
                    // Tag has a complete url in the form of https://domain.tld/tags/tag
                    const [, , hostname, ...pathSegments] = link.split('/');
                    const path = '/' + pathSegments.join('/');
                    const tag = path.indexOf("/tags/") !== -1 ? path.substring(path.indexOf("/tags/") + 6)
                        : link.startsWith('hashtag:/') ? link.substring(9)
                            : '';
                        tag.length > 0 ? pageStack.push(tagModelComponent, { hashtag: tag }): Qt.openUrlExternally(link);

                }
                color: Kirigami.Theme.textColor
                onHoveredLinkChanged: if (hoveredLink.length > 0) {
                    applicationWindow().hoverLinkIndicator.text = hoveredLink;
                } else {
                    applicationWindow().hoverLinkIndicator.text = "";
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // don't eat clicks on the Text
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
        }
    }

    onClicked: {
        Navigation.openAccount(root.userIdentity.id)
    }
}