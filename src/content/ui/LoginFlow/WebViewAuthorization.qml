// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtWebView

import org.kde.kirigami as Kirigami

import org.kde.tokodon

/**
 * Authorizing login within Tokodon itself with an internal browser.
 */
BaseAuthorization {
    id: root

    needsAuthLink: false

    actions: [
        Kirigami.Action {
            text: i18nc("@action:button Open in external web browser", "Open Externally")
            icon.name: "internet-services-symbolic"
            onTriggered: root.loginPage.openBrowserAuthPage()
        }
    ]

    WebView {
        id: webView

        Layout.fillWidth: true
        Layout.fillHeight: true

        url: root.account.authorizeUrl

        onLoadingChanged: (loadRequest) => {
            let urlObject = new URL(loadRequest.url);

            if (urlObject.protocol === "tokodon:") {
                webView.stop();
                root.setAuthCode(urlObject.searchParams.get("code"));
            }
        }
    }
}
