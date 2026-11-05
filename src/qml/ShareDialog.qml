// SPDX-FileCopyrightText: 2017 Atul Sharma <atulsharma406@gmail.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.ki18n
import org.kde.purpose as Purpose
import org.kde.notification
import org.kde.kirigami as Kirigami

Kirigami.Page {
    id: window

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None

    property alias index: jobView.index
    property alias model: jobView.model

    QQC2.Action {
        shortcut: 'Escape'
        onTriggered: window.Kirigami.PageStack.closeDialog()
    }

    Notification {
        id: sharingFailed
        eventId: "sharingFailed"
        text: KI18n.i18n("Sharing failed")
        urgency: Notification.NormalUrgency
    }

    Notification {
        id: sharingSuccess
        eventId: "sharingSuccess"
        flags: Notification.Persistent
    }

    Component.onCompleted: jobView.start()

    contentItem: Purpose.JobView {
        id: jobView
        onJobStateChanged: {
            if (jobView.jobState === Purpose.PurposeJobController.Finished) {
                window.Kirigami.PageStack.closeDialog()
            } else if (jobView.jobState === Purpose.PurposeJobController.Error) {
                // Show failure notification
                sharingFailed.sendEvent();

                window.Kirigami.PageStack.closeDialog()
            } else if (jobView.jobState === Purpose.PurposeJobController.Cancelled) {
                // Do nothing
                window.Kirigami.PageStack.closeDialog()
            }
        }
    }
}
