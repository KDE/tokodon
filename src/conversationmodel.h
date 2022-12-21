// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "abstractaccount.h"
#include "abstracttimelinemodel.h"

class Identity;
class Post;

struct Conversation
{
    QList<std::shared_ptr<Identity>> accounts;
    Post *lastPost;
    bool unread;
    QString id;
};

class ConversationModel : public AbstractTimelineModel
{
    Q_OBJECT

public:
    enum ExtraRole {
        UnreadRole = AbstractTimelineModel::ExtraRole + 1,
        ConversationIdRole,
    };
    Q_ENUM(ExtraRole)

    explicit ConversationModel(QObject *parent = nullptr);
    ~ConversationModel();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void markAsRead(const QString &id);

private:
    void fetchConversation(AbstractAccount *account);
    QList<Conversation> m_conversations;
};