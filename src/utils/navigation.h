// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QJSEngine>

#include "account/identity.h"
#include "timeline/post.h"

class Navigation : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static Navigation *create(QQmlEngine *, QJSEngine *)
    {
        auto inst = &instance();
        QJSEngine::setObjectOwnership(inst, QJSEngine::ObjectOwnership::CppOwnership);
        return inst;
    }

    static Navigation &instance();

    /**
     * @return If this is a debug build. Currently only controls if the auth code page is preferred.
     */
    Q_INVOKABLE bool isDebug() const;

    /**
     * @return If Tokodon was built with QtWebView support.
     */
    Q_INVOKABLE bool hasWebView() const;

Q_SIGNALS:
    void openComposer(const QString &text);
    void replyTo(Post *post);
    void openAccount(const QString &accountId);
    void openPost(const QString &postId);
    void openFullScreenImage(const QVariant &attachments, Identity *identity, int currentIndex);
    void openTag(const QString &tag);
    void reportPost(Identity *identity, const QString &postId);
    void reportUser(Identity *identity);
    void openList(const QString &listId, const QString &listName);

private:
    Navigation() = default;
};
