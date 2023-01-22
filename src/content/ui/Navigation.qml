// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma Singleton
import QtQml 2.14

QtObject {
    signal openStatusComposer()
    signal openAccount(accountId: string)
    signal openThread(postId: string)
    signal openFullScreenImage(attachments: var, currentIndex: int)
    signal openTag(tag: string)
}
