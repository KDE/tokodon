// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "abstractaccount.h"
#include "abstracttimelinemodel.h"

class Identity;
class Post;

struct Conversation {
    QList<std::shared_ptr<Identity>> accounts;
    Post *lastPost;
    bool unread;
    QString id;
};

/// Model used for direct messages (called Conversations in the application)
/// \see AbstractTimelineModel
class ConversationModel : public AbstractTimelineModel
{
    Q_OBJECT

public:
    /// Extra roles specifically for this model
    enum ExtraRole {
        UnreadRole = AbstractTimelineModel::ExtraRole + 1, ///< Number of unread messages
        ConversationAuthorsRole, ///< Human-readable list of accounts for this conversation
        ConversationIdRole, ///< Id for this conversation
    };
    Q_ENUM(ExtraRole)

    explicit ConversationModel(QObject *parent = nullptr);
    ~ConversationModel() override;

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    /// Mark the conversation of \p id as read
    Q_INVOKABLE void markAsRead(const QString &id);

private:
    void fetchConversation(AbstractAccount *account);
    QList<Conversation> m_conversations;
};