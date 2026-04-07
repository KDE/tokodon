// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigamiaddons.labs.components as KirigamiComponents
import org.kde.tokodon

/**
 * SearchField with a Popup to show autocompletion entries or search results
 */
KirigamiComponents.SearchPopupField {
    id: root

    property alias searchModel: searchPopup.model

    leftPadding: 0
    topPadding: 0
    bottomPadding: 0
    rightPadding: 0

    spaceAvailableLeft: false
    delaySearch: true

    searchField {
        Keys.onPressed: (event) => {
            if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return) {
                root.popup.close()
                Navigation.searchFor(root.text);
            }
            if (text.length === 0) {
                root.searchModel.clear();
            }
        }
    }
    onAccepted: searchModel.search(text)

    popupContentItem: SearchView {
        id: searchPopup
        text: root.text
        onItemSelected: root.popup.close()
        clip: true
    }
}
