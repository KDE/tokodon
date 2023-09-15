// SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2023 Mathis Brüchert <mbb@kaidan.im>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
//
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls 2 as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2 as Kirigami
import org.kde.kirigamiaddons.labs.components 1 as KirigamiComponents
import org.kde.tokodon

/**
 * SearchField with a Popup to show autocompletion entries or search results
 */
KirigamiComponents.SearchPopupField {
    id: root

    property alias searchModel: searchView.model

    leftPadding: 0
    topPadding: 0
    bottomPadding: 0
    rightPadding: 0

    spaceAvailableLeft: false

    delaySearch: true
    onAccepted: searchModel.search(text)

    popupContentItem: SearchView {
        id: searchView
        text: root.text
    }
}
