// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "conversation/conversationmodel.h"

#include "networkcontroller.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>

#include <QTextDocumentFragment>

using namespace Qt::Literals::StringLiterals;

ConversationModel::ConversationModel(QObject *parent)
    : AbstractTimelineModel(parent)
{
    if (AccountManager::instance().selectedAccount()) {
        fetchConversation(AccountManager::instance().selectedAccount());
    }
    connect(&AccountManager::instance(), &AccountManager::accountSelected, this, [this](AbstractAccount *account) {
        fetchConversation(account);
    });
}

ConversationModel::~ConversationModel() = default;

QHash<int, QByteArray> ConversationModel::roleNames() const
{
    auto roles = AbstractTimelineModel::roleNames();
    roles.insert(UnreadRole, QByteArrayLiteral("unread"));
    roles.insert(ConversationIdRole, QByteArrayLiteral("conversationId"));
    return roles;
}

int ConversationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_conversations.count();
}

QVariant ConversationModel::data(const QModelIndex &index, int role) const
{
    const auto row = index.row();
    const auto lastPost = m_conversations[row].lastPost;
    const auto &identities = m_conversations[row].accounts;
    const auto &firstIdentity = identities[0];

    switch (role) {
    case AuthorIdentityRole:
        return QVariant::fromValue<Identity *>(firstIdentity.get());
    case ConversationIdRole:
        return m_conversations[row].id;
    case UnreadRole:
        return m_conversations[row].unread;
    case ContentRole:
        return QTextDocumentFragment::fromHtml(lastPost->content()).toPlainText();
    case ConversationAuthorsRole:
        if (identities.count() == 0) {
            return i18n("Empty conversation");
        } else if (identities.count() == 1) {
            return firstIdentity->displayNameHtml();
        } else if (identities.count() == 2) {
            return i18n("%1 and %2", firstIdentity->displayNameHtml(), identities[2]->displayNameHtml());
        } else {
            return i18np("%2 and one other", "%2 and %1 others", identities.count() - 1, firstIdentity->displayNameHtml());
        }
    default:
        return postData(lastPost, role);
    }
}

void ConversationModel::fetchConversation(AbstractAccount *account)
{
    setLoading(true);

    account->get(
        account->apiUrl(QStringLiteral("/api/v1/conversations")),
        true,
        this,
        [account, this](QNetworkReply *reply) {
            beginResetModel();
            m_conversations.clear();
            const auto conversationArray = QJsonDocument::fromJson(reply->readAll()).array();
            for (const auto &conversation : conversationArray) {
                const auto obj = conversation.toObject();
                const auto accountsArray = obj["accounts"_L1].toArray();
                QList<std::shared_ptr<Identity>> accounts;
                std::ranges::transform(std::as_const(accountsArray), std::back_inserter(accounts), [account](const QJsonValue &value) -> auto {
                    const auto accountObj = value.toObject();
                    return account->identityLookup(accountObj["id"_L1].toString(), accountObj);
                });
                m_conversations.append(Conversation{
                    accounts,
                    new Post(account, obj["last_status"_L1].toObject(), this),
                    obj["unread"_L1].toBool(),
                    obj["id"_L1].toString(),
                });
            }
            setLoading(false);
            endResetModel();
        },
        [this](QNetworkReply *reply) {
            setLoading(false);
            Q_EMIT NetworkController::instance().networkErrorOccurred(reply->errorString());
        });
}

void ConversationModel::markAsRead(const QString &id)
{
    auto account = AccountManager::instance().selectedAccount();

    account->post(account->apiUrl(QStringLiteral("/api/v1/conversations/%1/read").arg(id)), QJsonDocument(), true, this, [id, this](QNetworkReply *reply) {
        const auto conversationObj = QJsonDocument::fromJson(reply->readAll()).object();
        int i = 0;
        for (auto &conversation : m_conversations) {
            if (conversation.id == id) {
                conversation.unread = false;
                Q_EMIT dataChanged(index(i, 0), index(i, 0), {UnreadRole});
                break;
            }
            i++;
        }
    });
}

#include "moc_conversationmodel.cpp"
