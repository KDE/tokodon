// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "timeline/post.h"

class ListEditorBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString listId READ listId WRITE setListId NOTIFY listIdChanged)
    Q_PROPERTY(QString title MEMBER m_title NOTIFY titleChanged)
    Q_PROPERTY(bool exclusive MEMBER m_exclusive NOTIFY exclusiveChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool favorite MEMBER m_favorite NOTIFY favoriteChanged)

public:
    explicit ListEditorBackend(QObject *parent = nullptr);

    [[nodiscard]] QString listId() const;
    void setListId(const QString &listId);

    [[nodiscard]] bool loading() const;

public Q_SLOTS:
    void submit();
    void deleteList();
    QStringList replyPolicies();
    [[nodiscard]] int replyPolicyIndex() const;
    void setReplyPolicyIndex(int index);

Q_SIGNALS:
    void listIdChanged();
    void titleChanged();
    void exclusiveChanged();
    void done();
    void loadingChanged();
    void favoriteChanged();

private:
    QString m_listId;
    QString m_title;
    QString m_repliesPolicy = QStringLiteral("none");
    bool m_exclusive = false;
    bool m_loading = false;
    bool m_favorite = false;
};
