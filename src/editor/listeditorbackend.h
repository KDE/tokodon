// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "post.h"

class ListEditorBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString listId READ listId WRITE setListId NOTIFY listIdChanged)
    Q_PROPERTY(QString title MEMBER m_title NOTIFY titleChanged)
    Q_PROPERTY(bool exclusive MEMBER m_exclusive NOTIFY exclusiveChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    explicit ListEditorBackend(QObject *parent = nullptr);

    QString listId() const;
    void setListId(const QString &listId);

    bool loading() const;

public Q_SLOTS:
    void submit();
    void deleteList();
    QStringList replyPolicies();
    int replyPolicyIndex() const;
    void setReplyPolicyIndex(int index);

Q_SIGNALS:
    void listIdChanged();
    void titleChanged();
    void exclusiveChanged();
    void done();
    void loadingChanged();

private:
    QString m_listId;
    QString m_title;
    QString m_repliesPolicy = QStringLiteral("none");
    bool m_exclusive = false;
    bool m_loading = false;
};
