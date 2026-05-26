// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/socialgraphmodel.h"

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "account/relationship.h"
#include "networkcontroller.h"
#include "texthandler.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>

using namespace Qt::Literals::StringLiterals;

SocialGraphModel::SocialGraphModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QString SocialGraphModel::name() const
{
    return m_followListName;
}

QString SocialGraphModel::displayName() const
{
    if (m_followListName == QStringLiteral("request")) {
        return i18nc("@title", "Follow Requests");
    }
    if (m_followListName == QStringLiteral("followers")) {
        return i18nc("@title", "Followers");
    }
    if (m_followListName == QStringLiteral("following")) {
        return i18nc("@title", "Following");
    }
    if (m_followListName == QStringLiteral("mutes")) {
        return i18nc("@title", "Muted Users");
    }
    if (m_followListName == QStringLiteral("blocks")) {
        return i18nc("@title", "Blocked Users");
    }
    if (m_followListName == QStringLiteral("featured")) {
        return i18nc("@title", "Featured Users");
    }
    if (m_followListName == QStringLiteral("favourited_by")) {
        return i18ncp("@title", "%1 favorite", "%1 favorites", m_count);
    }
    if (m_followListName == QStringLiteral("reblogged_by")) {
        return i18ncp("@title", "%1 boost", "%1 boosts", m_count);
    }
    if (m_followListName == QStringLiteral("familiar_followers")) {
        return i18nc("@title", "Familiar Followers");
    }
    if (m_followListName == QStringLiteral("list")) {
        return i18nc("@title", "Manage List Users");
    }
    if (m_followListName == QStringLiteral("collection")) {
        return i18nc("@title", "Manage Collection Users");
    }
    return {};
}

QString SocialGraphModel::placeholderText() const
{
    if (m_followListName == QStringLiteral("request")) {
        return i18n("No Follow Requests");
    }
    if (m_followListName == QStringLiteral("followers")) {
        return i18n("No Followers");
    }
    if (m_followListName == QStringLiteral("following")) {
        return i18n("No Followed Users");
    }
    if (m_followListName == QStringLiteral("mutes")) {
        return i18n("No Muted Users");
    }
    if (m_followListName == QStringLiteral("blocks")) {
        return i18n("No Blocked Users");
    }
    if (m_followListName == QStringLiteral("featured")) {
        return i18n("No Featured Users");
    }
    if (m_followListName == QStringLiteral("favourited_by")) {
        return i18n("No Users Favorited This Post");
    }
    if (m_followListName == QStringLiteral("reblogged_by")) {
        return i18n("No Users Boosted This Post");
    }
    if (m_followListName == QStringLiteral("familiar_followers")) {
        return i18n("No Familiar Followers");
    }
    if (m_followListName == QStringLiteral("list") || m_followListName == QStringLiteral("collection")) {
        return i18n("No Users");
    }
    return {};
}

QString SocialGraphModel::placeholderExplanation() const
{
    if (m_followListName == QStringLiteral("request")) {
        return i18n("Some users need to be manually approved before they can follow you, and will show up here.");
    }
    if (m_followListName == QStringLiteral("followers")) {
        return i18nc("@info:placeholder", "No one is following this user, or they have chosen to hide this information.");
    }
    if (m_followListName == QStringLiteral("following")) {
        return i18nc("@info:placeholder", "This user isn't following anyone, or has chosen to hide this information.");
    }
    if (m_followListName == QStringLiteral("mutes")) {
        return i18n("Mute users to stop them from showing up in your notifications.");
    }
    if (m_followListName == QStringLiteral("blocks")) {
        return i18n("Block users to hide the user from your timeline entirely.");
    }
    if (m_followListName == QStringLiteral("featured")) {
        return i18n("Users you like can be featured and shown on your profile.");
    }
    if (m_followListName == QStringLiteral("favourited_by")) {
        return i18n("No one has favorited this post yet. Maybe you'll be the first?");
    }
    if (m_followListName == QStringLiteral("reblogged_by")) {
        return i18n("No one has boosted this post yet. You should boost it to expand its reach!");
    }
    if (m_followListName == QStringLiteral("familiar_followers")) {
        return {};
    }
    if (m_followListName == QStringLiteral("list")) {
        return i18n("There is no one in this list yet.");
    }
    if (m_followListName == QStringLiteral("collection")) {
        return i18n("There is no one in this collection yet.");
    }
    return {};
}

QString SocialGraphModel::placeholderIconName() const
{
    if (m_followListName == QStringLiteral("request")) {
        return QStringLiteral("list-add-user");
    }
    if (m_followListName == QStringLiteral("followers")) {
        return QStringLiteral("list-add-user");
    }
    if (m_followListName == QStringLiteral("following")) {
        return QStringLiteral("list-add-user");
    }
    if (m_followListName == QStringLiteral("mutes")) {
        return QStringLiteral("microphone-sensitivity-muted");
    }
    if (m_followListName == QStringLiteral("blocks")) {
        return QStringLiteral("cards-block");
    }
    if (m_followListName == QStringLiteral("featured") || m_followListName == QStringLiteral("favourited_by")) {
        return QStringLiteral("favorite");
    }
    if (m_followListName == QStringLiteral("reblogged_by")) {
        return QStringLiteral("boost");
    }
    if (m_followListName == QStringLiteral("familiar_followers") || m_followListName == QStringLiteral("list")
        || m_followListName == QStringLiteral("collection")) {
        return QStringLiteral("list-add-user");
    }
    return {};
}

void SocialGraphModel::setName(const QString &followlistname)
{
    if (followlistname == m_followListName) {
        return;
    }

    m_followListName = followlistname;
    Q_EMIT nameChanged();
    fillTimeline();
}

QString SocialGraphModel::accountId() const
{
    return m_accountId;
}

void SocialGraphModel::setAccountId(const QString &accountId)
{
    m_accountId = accountId;
    Q_EMIT accountIdChanged();
    reset();
    fillTimeline();
}

QString SocialGraphModel::statusId() const
{
    return m_statusId;
}

void SocialGraphModel::setStatusId(const QString &statusId)
{
    m_statusId = statusId;
    Q_EMIT statusIdChanged();
    reset();
    fillTimeline();
}

int SocialGraphModel::count() const
{
    return m_count;
}

void SocialGraphModel::setCount(int count)
{
    m_count = count;
}

QString SocialGraphModel::listId() const
{
    return m_listId;
}

void SocialGraphModel::setListId(const QString &listId)
{
    m_listId = listId;
    Q_EMIT listIdChanged();
    reset();
    fillTimeline();
}

QString SocialGraphModel::collectionId() const
{
    return m_collectionId;
}

void SocialGraphModel::setCollectionId(const QString &collectionId)
{
    m_collectionId = collectionId;
    Q_EMIT collectionIdChanged();
    reset();
    fillTimeline();
}

QVariant SocialGraphModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto identity = m_accounts[index.row()].get();
    switch (role) {
    case IdentityRole:
        return QVariant::fromValue<Identity *>(identity);
    case LastStatusAtRole:
        return identity->lastStatusAt();
    case RelativeTimeRole:
        return TextHandler::getRelativeDate(identity->lastStatusAt());
    default:
        Q_UNREACHABLE();
    }
}

int SocialGraphModel::rowCount(const QModelIndex &) const
{
    return m_accounts.count();
}

QHash<int, QByteArray> SocialGraphModel::roleNames() const
{
    return {{IdentityRole, "identity"}, {LastStatusAtRole, "lastStatusAt"}, {RelativeTimeRole, "relativeTime"}};
}

bool SocialGraphModel::loading() const
{
    return m_loading;
}

void SocialGraphModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

bool SocialGraphModel::isFollowRequest() const
{
    return m_followListName == "request"_L1;
}

bool SocialGraphModel::isFollowing() const
{
    return m_followListName == "following"_L1;
}

bool SocialGraphModel::isFollower() const
{
    return m_followListName == "followers"_L1;
}

bool SocialGraphModel::isList() const
{
    return m_followListName == "list"_L1;
}

bool SocialGraphModel::isBlockList() const
{
    return m_followListName == "blocks"_L1;
}

bool SocialGraphModel::isMuteList() const
{
    return m_followListName == "mutes"_L1;
}

bool SocialGraphModel::isCollection() const
{
    return m_followListName == "collection"_L1;
}

void SocialGraphModel::actionAllow(const QModelIndex &index)
{
    auto account = AccountManager::instance().selectedAccount();

    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid))
        return;

    auto requestIdentity = m_accounts[index.row()].get();
    const auto requestIdentityId = requestIdentity->id();

    account->post(account->apiUrl(QStringLiteral("/api/v1/follow_requests/%1/authorize").arg(requestIdentityId)),
                  QJsonDocument{},
                  true,
                  this,
                  [this, account, requestIdentity, index](QNetworkReply *reply) {
                      const auto newRelation = QJsonDocument::fromJson(reply->readAll()).object();

                      m_accounts[index.row()]->setRelationship(new Relationship(requestIdentity, newRelation));

                      beginRemoveRows(QModelIndex(), index.row(), index.row());
                      m_accounts.removeAt(index.row());
                      endRemoveRows();

                      account->checkForFollowRequests();
                  });
}

void SocialGraphModel::actionDeny(const QModelIndex &index)
{
    auto account = AccountManager::instance().selectedAccount();

    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid))
        return;

    auto requestIdentity = m_accounts[index.row()].get();
    const auto requestIdentityId = requestIdentity->id();

    account->post(account->apiUrl(QStringLiteral("/api/v1/follow_requests/%1/reject").arg(requestIdentityId)),
                  QJsonDocument{},
                  true,
                  this,
                  [this, account, requestIdentity, index](QNetworkReply *reply) {
                      const auto newRelation = QJsonDocument::fromJson(reply->readAll()).object();

                      m_accounts[index.row()]->setRelationship(new Relationship(requestIdentity, newRelation));

                      beginRemoveRows(QModelIndex(), index.row(), index.row());
                      m_accounts.removeAt(index.row());
                      endRemoveRows();

                      account->checkForFollowRequests();
                  });
}

void SocialGraphModel::actionUnfollow(const QModelIndex &index)
{
    auto account = AccountManager::instance().selectedAccount();

    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid))
        return;

    auto requestIdentity = m_accounts[index.row()].get();
    account->unfollowAccount(requestIdentity);
}

void SocialGraphModel::actionRemoveFollower(const QModelIndex &index)
{
    auto account = AccountManager::instance().selectedAccount();

    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid))
        return;

    auto requestIdentity = m_accounts[index.row()].get();
    account->removeFollower(requestIdentity);
}

void SocialGraphModel::actionRemoveFromList(const QModelIndex &index)
{
    auto account = AccountManager::instance().selectedAccount();

    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid) || m_listId.isEmpty()) {
        return;
    }

    auto requestIdentity = m_accounts[index.row()].get();

    const QUrlQuery query{{QStringLiteral("account_ids[]"), requestIdentity->id()}};

    auto url = account->apiUrl(QStringLiteral("/api/v1/lists/%1/accounts").arg(m_listId));
    url.setQuery(query);

    account->deleteResource(url, true, this, [this, index](QNetworkReply *reply) {
        Q_UNUSED(reply)

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        m_accounts.removeAt(index.row());
        endRemoveRows();
    });
}

void SocialGraphModel::actionAddToList(const QString &accountId)
{
    auto account = AccountManager::instance().selectedAccount();

    if (m_listId.isEmpty()) {
        return;
    }

    const QUrlQuery query{{QStringLiteral("account_ids[]"), accountId}};

    const auto url = account->apiUrl(QStringLiteral("/api/v1/lists/%1/accounts").arg(m_listId));
    account->post(url, query, true, this, [this, account, accountId](QNetworkReply *reply) {
        Q_UNUSED(reply)

        beginInsertRows(QModelIndex(), m_accounts.size(), m_accounts.size());
        m_accounts.push_back(account->identityLookup(accountId, {}));
        endInsertRows();
    });
}

void SocialGraphModel::actionUnblock(const QModelIndex &index)
{
    auto account = AccountManager::instance().selectedAccount();

    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid))
        return;

    auto requestIdentity = m_accounts[index.row()].get();
    account->unblockAccount(requestIdentity);

    beginRemoveRows({}, index.row(), index.row());
    m_accounts.removeAt(index.row());
    endRemoveRows();
}

void SocialGraphModel::actionUnmute(const QModelIndex &index)
{
    auto account = AccountManager::instance().selectedAccount();

    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid))
        return;

    auto requestIdentity = m_accounts[index.row()].get();
    account->unmuteAccount(requestIdentity);

    beginRemoveRows({}, index.row(), index.row());
    m_accounts.removeAt(index.row());
    endRemoveRows();
}

void SocialGraphModel::actionAddToCollection(const QString &accountId)
{
    auto account = AccountManager::instance().selectedAccount();

    if (m_collectionId.isEmpty()) {
        return;
    }

    const QUrlQuery query{{QStringLiteral("account_id"), accountId}};

    const auto url = account->apiUrl(QStringLiteral("/api/v1/collections/%1/items").arg(m_collectionId));
    account->post(url, query, true, this, [this, account, accountId](QNetworkReply *reply) {
        Q_UNUSED(reply)

        beginInsertRows(QModelIndex(), m_accounts.size(), m_accounts.size());
        m_accounts.push_back(account->identityLookup(accountId, {}));
        endInsertRows();
    });
}

bool SocialGraphModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_next.has_value() && !loading();
}

void SocialGraphModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    fillTimeline();
}

void SocialGraphModel::fillTimeline()
{
    auto account = AccountManager::instance().selectedAccount();

    if (m_followListName.isEmpty() || m_followListName.isNull()) {
        return;
    }

    if ((m_followListName == QStringLiteral("followers") || m_followListName == QStringLiteral("following")
         || m_followListName == QStringLiteral("familiar_followers") || m_followListName == QStringLiteral("featured"))
        && (m_accountId.isEmpty() || m_accountId.isNull())) {
        return;
    }

    if ((m_followListName == QStringLiteral("favourited_by") || m_followListName == QStringLiteral("reblogged_by"))
        && (m_statusId.isEmpty() || m_statusId.isNull())) {
        return;
    }

    if (m_followListName == QStringLiteral("list") && (m_listId.isEmpty() || m_listId.isNull())) {
        return;
    }

    if (m_followListName == QStringLiteral("collection") && (m_collectionId.isEmpty() || m_collectionId.isNull())) {
        return;
    }

    if (m_loading) {
        return;
    }
    setLoading(true);

    QString uri;
    if (m_followListName == QStringLiteral("request")) {
        uri = QStringLiteral("/api/v1/follow_requests");
    } else if (m_followListName == QStringLiteral("followers")) {
        uri = QStringLiteral("/api/v1/accounts/%1/followers").arg(m_accountId);
    } else if (m_followListName == QStringLiteral("following")) {
        uri = QStringLiteral("/api/v1/accounts/%1/following").arg(m_accountId);
    } else if (m_followListName == QStringLiteral("mutes")) {
        uri = QStringLiteral("/api/v1/mutes");
    } else if (m_followListName == QStringLiteral("blocks")) {
        uri = QStringLiteral("/api/v1/blocks");
    } else if (m_followListName == QStringLiteral("featured")) {
        uri = QStringLiteral("/api/v1/accounts/%1/endorsements").arg(m_accountId);
    } else if (m_followListName == QStringLiteral("favourited_by")) {
        uri = QStringLiteral("/api/v1/statuses/%1/favourited_by").arg(m_statusId);
    } else if (m_followListName == QStringLiteral("reblogged_by")) {
        uri = QStringLiteral("/api/v1/statuses/%1/reblogged_by").arg(m_statusId);
    } else if (m_followListName == QStringLiteral("familiar_followers")) {
        uri = QStringLiteral("/api/v1/accounts/familiar_followers");
    } else if (m_followListName == QStringLiteral("list")) {
        uri = QStringLiteral("/api/v1/lists/%1/accounts").arg(m_listId);
    } else if (m_followListName == QStringLiteral("collection")) {
        uri = QStringLiteral("/api/v1/collections/%1").arg(m_collectionId);
    }

    QUrl url;
    if (!m_next) {
        url = account->apiUrl(uri);
    } else {
        url = m_next.value();
    }

    if (m_followListName == QStringLiteral("familiar_followers")) {
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("id"), m_accountId);
        url.setQuery(query);
    }

    account->get(
        url,
        true,
        this,
        [this, account](QNetworkReply *reply) {
            const auto followRequestResult = QJsonDocument::fromJson(reply->readAll());
            auto accounts = followRequestResult.array();

            if (m_followListName == QStringLiteral("collection")) {
                accounts = followRequestResult.object()["accounts"_L1].toArray();
                accounts.pop_front(); // Remove first account which is the creator
            }

            if (!accounts.isEmpty()) {
                const auto linkHeader = QString::fromUtf8(reply->rawHeader(QByteArrayLiteral("Link")));
                m_next = TextHandler::getNextLink(linkHeader);

                QList<std::shared_ptr<Identity>> fetchedAccounts;
                QJsonArray value = accounts;

                // This is a list of FamiliarFollower, not Account. So we need to transform it first.
                if (m_followListName == QStringLiteral("familiar_followers")) {
                    value = accounts.first()["accounts"_L1].toArray();
                }

                std::ranges::transform(std::as_const(value), std::back_inserter(fetchedAccounts), [account](const QJsonValue &value) -> auto {
                    const auto identityJson = value.toObject();
                    return account->identityLookup(identityJson["id"_L1].toString(), identityJson);
                });

                size_t i = m_accounts.size();
                for (auto &identity : fetchedAccounts) {
                    connect(identity.get(), &Identity::relationshipChanged, this, [this, i, identity] {
                        bool shouldRemove = false;
                        if (isFollowing()) {
                            shouldRemove = identity->relationship() != nullptr ? !identity->relationship()->following() : true;
                        } else if (isFollower()) {
                            shouldRemove = identity->relationship() != nullptr ? identity->relationship()->following() : true;
                        }

                        if (shouldRemove) {
                            beginRemoveRows({}, i, i);
                            m_accounts.removeAt(i);
                            endRemoveRows();
                        }
                    });
                    i++;
                }

                beginInsertRows({}, m_accounts.size(), m_accounts.size() + fetchedAccounts.size() - 1);
                m_accounts += fetchedAccounts;
                endInsertRows();
            }

            setLoading(false);
        },
        [this](QNetworkReply *reply) {
            setLoading(false);
            Q_EMIT networkErrorOccurred(reply->errorString());
        });
}

void SocialGraphModel::reset()
{
    beginResetModel();
    m_accounts.clear();
    endResetModel();
}

#include "moc_socialgraphmodel.cpp"
