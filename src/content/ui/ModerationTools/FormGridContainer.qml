/*
 * Copyright 2022 Devin Lin <devin@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami

/**
 * A single container for a grid that is contained in a form.
 *
 * The height will take the implicit height of the contentItem, while the width
 * is expected to be given by the parent.
 */
Item {
    id: root

    /**
     * The contents of the form card.
     */
    property Item contentItem: Item {}

    /**
     * The maximum width of the card.
     */
    property real maximumWidth: Kirigami.Units.gridUnit * 30

    property real padding: 0
    property real verticalPadding: padding
    property real horizontalPadding: padding
    property real topPadding: verticalPadding
    property real bottomPadding: verticalPadding
    property real leftPadding: horizontalPadding
    property real rightPadding: horizontalPadding

    /**
     * Whether the card's width is being restricted.
     */
    readonly property bool cardWidthRestricted: root.width > root.maximumWidth

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    implicitHeight: topPadding + bottomPadding + contentItem.implicitHeight

    Item {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: parent.right

            leftMargin: root.cardWidthRestricted ? Math.round((root.width - root.maximumWidth) / 2) : -1
            rightMargin: root.cardWidthRestricted ? Math.round((root.width - root.maximumWidth) / 2) : -1
        }

        Item {
            id: contentItemLoader
            property var contentItem: root.contentItem

            onContentItemChanged: {
                // clear old items
                contentItemLoader.children = "";

                if (contentItem instanceof Item) {
                    contentItem.parent = contentItemLoader;
                    contentItem.anchors.fill = contentItemLoader;
                    contentItemLoader.children.push(contentItem);
                }
            }

            // add 1 to margins to account for the border (so content doesn't overlap it)
            anchors {
                fill: parent
                leftMargin: root.leftPadding
                rightMargin: root.rightPadding
                topMargin: root.topPadding
                bottomMargin: root.bottomPadding
            }
        }
    }
}
