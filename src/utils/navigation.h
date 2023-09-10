// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class Identity;
class Post;

class Navigation : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void openStatusComposer();
    void replyTo(const QString &inReplyTo, const QVariant &mentions, int visibility, Identity *authorIdentity, Post *post);
    void openAccount(const QString &accountId);
    void openThread(const QString &postId);
    void openFullScreenImage(const QVariant &attachments, Identity *identity, int currentIndex);
    void openTag(const QString &tag);
};
