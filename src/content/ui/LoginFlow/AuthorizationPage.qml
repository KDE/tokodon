// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtWebView

import org.kde.kirigami 2 as Kirigami

import org.kde.tokodon

Kirigami.Page {
    id: root

    required property var account

    title: i18nc("@title:window", "Authorization")

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    function setAuthCode(authCode) {
        account.setToken(authCode);
        pageStack.layers.clear();
        pageStack.replace(mainTimeline, {
            name: "home"
        });
        if (root.Window.window !== applicationWindow()) {
            root.Window.window.close();
        }
    }

    WebView {
        id: webView

        anchors.fill: parent

        url: root.account.authorizeUrl

        onLoadingChanged: (loadRequest) => {
            let urlObject = new URL(loadRequest.url);

            if (urlObject.protocol === "tokodon:") {
                webView.stop();
                root.setAuthCode(urlObject.searchParams.get("code"));
            }
        }
    }

    data: Connections {
        target: Controller

        function onReceivedAuthCode(authCode) {
            root.setAuthCode(authCode);
        }
    }
}
