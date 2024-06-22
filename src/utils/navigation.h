// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "account/identity.h"
#include "timeline/post.h"

/**
 * @brief Deals with navigation in the QML pages but also allows C++ to control some of the navigation too.
 */
class Navigation final : public QObject
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
    /**
     * @brief Opens the composer UI with @p text.
     */
    void openComposer(const QString &text);

    /**
     * @brief Opens the composer UI to reply to @p post.
     */
    void replyTo(Post *post);

    /**
     * @brief Opens the profile page for @p accountId.
     */
    void openAccount(const QString &accountId);

    /**
     * @brief Opens the thread page for @p postId.
     */
    void openPost(const QString &postId);

    /**
     * @brief Opens the fullscreen image viewer.
     * @param attachments List of attachments.
     * @param identity The identity of the post author.
     * @param currentIndex The index of the attachment to open on.
     */
    void openFullScreenImage(const QVariant &attachments, Identity *identity, int currentIndex);

    /**
     * @brief Opens the tag page for @p tag.
     */
    void openTag(const QString &tag);

    /**
     * @brief Opens the report dialog.
     * @param identity The identity of the post author.
     * @param postId The post's id.
     */
    void reportPost(Identity *identity, const QString &postId);

    /**
     * @brief Opens the report dialog for the account of @p identity.
     */
    void reportUser(Identity *identity);

    /**
     * @brief Opens the list page.
     * @param listId The list id.
     * @param listName The name assigned to the list.
     */
    void openList(const QString &listId, const QString &listName);

private:
    Navigation() = default;
};
