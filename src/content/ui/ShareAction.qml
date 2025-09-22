// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami 2 as Kirigami

/**
 * Action that allows an user to share data with other apps and service
 * installed on their computer. The goal of this high level API is to
 * adapte itself for each platform and adopt the native component.
 *
 * TODO add Android support
 */
Kirigami.Action {
    id: root

    icon.name: "emblem-shared-symbolic"
    text: i18n("Share")
    tooltip: i18n("Share the selected media")

    visible: false

    /**
     * This property holds the input data for purpose.
     *
     * @code{.qml}
     * Purpose.ShareAction {
     *     inputData: {
     *         'urls': ['file://home/notroot/Pictures/mypicture.png'],
     *         'mimeType': ['image/png']
     *     }
     * }
     * @endcode
     */
    property var inputData: ({})

    property Instantiator _instantiator: Instantiator {
        Component.onCompleted: {
            const purposeModel = Qt.createQmlObject(`import org.kde.purpose 1.0 as Purpose;
Purpose.PurposeAlternativesModel {
    pluginType: "ShareUrl"
    // Copying to clipboard rarely works, due to how Mastodon and most Fediverse software behave and are dependent on JavaScript.
    // We also already have a way to copy the URL to the clipboard, so having a second way is just confusing.
    disabledPlugins: ["clipboardplugin", "tokodonplugin"]
}`, root._instantiator);
            purposeModel.inputData = Qt.binding(function() {
                return root.inputData;
            });
            root._instantiator.model = purposeModel;
            root.visible = true;
        }

        delegate: Kirigami.Action {
            required property int index
            required property string actionDisplay
            required property string iconName

            text: actionDisplay
            icon.name: iconName
            onTriggered: {
                applicationWindow().pageStack.pushDialogLayer(Qt.createComponent("org.kde.tokodon", "ShareDialog"), {
                    title: root.tooltip,
                    index: index,
                    model: root._instantiator.model
                })
            }
        }
        onObjectAdded: (index, object) => {
            object.index = index;
            root.children.push(object)
        }
        onObjectRemoved: (index, object) => root.children = Array.from(root.children).filter(obj => obj.pluginId !== object.pluginId)
    }
}
