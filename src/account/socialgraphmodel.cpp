// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "socialgraphmodel.h"

#include "identity.h"

#include "abstractaccount.h"
#include "accountmanager.h"
#include "relationship.h"
#include <KLocalizedString>
#include <QFile>
#include <QJsonDocument>
#include <QNetworkReply>
#include <qstringliteral.h>

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
    if (m_followListName == "request") {
        return i18nc("@title", "Follow Requests");
    } else if (m_followListName == "followers") {
        return i18nc("@title", "Followers");
    } else if (m_followListName == "following") {
        return i18nc("@title", "Following");
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
    fillTimeline();
}

QVariant SocialGraphModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto identity = m_accounts[index.row()].get();
    switch (role) {
    case CustomRoles::IdentityRole:
        return QVariant::fromValue<Identity *>(identity);
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
    return {
        {CustomRoles::IdentityRole, "identity"},
    };
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
    return m_followListName == "request";
}

bool SocialGraphModel::isFollowing() const
{
    return m_followListName == "following";
}

bool SocialGraphModel::isFollower() const
{
    return m_followListName == "followers";
}

void SocialGraphModel::actionAllow(const QModelIndex &index)
{
    auto account = AccountManager::instance().selectedAccount();

    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid))
        return;

    auto requestIdentity = m_accounts[index.row()].get();
    const auto requestIdentityId = requestIdentity->id();

    account->post(account->apiUrl(QString("/api/v1/follow_requests/%1/authorize").arg(requestIdentityId)),
                  QJsonDocument{},
                  true,
                  this,
                  [this, requestIdentity, index](QNetworkReply *reply) {
                      const auto newRelation = QJsonDocument::fromJson(reply->readAll()).object();

                      m_accounts[index.row()]->setRelationship(new Relationship(requestIdentity, newRelation));

                      beginRemoveRows(QModelIndex(), index.row(), index.row());
                      m_accounts.removeAt(index.row());
                      endRemoveRows();
                  });
}

void SocialGraphModel::actionDeny(const QModelIndex &index)
{
    auto account = AccountManager::instance().selectedAccount();

    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid))
        return;

    auto requestIdentity = m_accounts[index.row()].get();
    const auto requestIdentityId = requestIdentity->id();

    account->post(account->apiUrl(QString("/api/v1/follow_requests/%1/reject").arg(requestIdentityId)),
                  QJsonDocument{},
                  true,
                  this,
                  [this, requestIdentity, index](QNetworkReply *reply) {
                      const auto newRelation = QJsonDocument::fromJson(reply->readAll()).object();

                      m_accounts[index.row()]->setRelationship(new Relationship(requestIdentity, newRelation));

                      beginRemoveRows(QModelIndex(), index.row(), index.row());
                      m_accounts.removeAt(index.row());
                      endRemoveRows();
                  });
}

bool SocialGraphModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return !m_next.isEmpty();
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

    if ((m_followListName == "followers" || m_followListName == "following") && (m_accountId.isEmpty() || m_accountId.isNull())) {
        return;
    }

    if (m_loading) {
        return;
    }
    setLoading(true);

    QString uri;
    if (m_followListName == "request") {
        uri = "/api/v1/follow_requests";
    } else if (m_followListName == "followers") {
        uri = QStringLiteral("/api/v1/accounts/%1/followers").arg(m_accountId);
    } else if (m_followListName == "following") {
        uri = QStringLiteral("/api/v1/accounts/%1/following").arg(m_accountId);
    } else if (m_followListName == "mutes") {
        uri = QStringLiteral("/api/v1/mutes");
    } else if (m_followListName == "blocks") {
        uri = QStringLiteral("/api/v1/blocks");
    }

    QUrl url;
    if (m_next.isEmpty()) {
        url = account->apiUrl(uri);
    } else {
        url = m_next;
    }

    account->get(url, true, this, [this, account](QNetworkReply *reply) {
        const auto followRequestResult = QJsonDocument::fromJson(reply->readAll());
        const auto accounts = followRequestResult.array();

        if (!accounts.isEmpty()) {
            static QRegularExpression re("<(.*)>; rel=\"next\"");
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(next);
            if (re.isValid()) {
                m_next = QUrl::fromUserInput(match.captured(1));
            }

            QList<std::shared_ptr<Identity>> fetchedAccounts;

            std::transform(
                accounts.cbegin(),
                accounts.cend(),
                std::back_inserter(fetchedAccounts),
                [account](const QJsonValue &value) -> auto{
                    const auto identityJson = value.toObject();
                    return account->identityLookup(identityJson["id"].toString(), identityJson);
                });

            beginInsertRows({}, m_accounts.size(), m_accounts.size() + fetchedAccounts.size() - 1);
            m_accounts += fetchedAccounts;
            endInsertRows();
        }

        setLoading(false);
    });
}
