// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "followrequestmodel.h"

#include "identity.h"

#include "abstractaccount.h"
#include "accountmanager.h"
#include "relationship.h"
#include <QFile>
#include <QJsonDocument>
#include <QNetworkReply>

FollowRequestModel::FollowRequestModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fillTimeline();
}

QVariant FollowRequestModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid))
        return {};

    const auto identity = m_accounts[index.row()].get();

    switch (role) {
    case CustomRoles::IdentityRole:
        return QVariant::fromValue<Identity *>(identity);
    default:
        return {};
    }
}

int FollowRequestModel::rowCount(const QModelIndex &) const
{
    return m_accounts.count();
}

QHash<int, QByteArray> FollowRequestModel::roleNames() const
{
    return {{CustomRoles::IdentityRole, "identity"}};
}

bool FollowRequestModel::loading() const
{
    return m_loading;
}

void FollowRequestModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

void FollowRequestModel::actionAllow(const QModelIndex &index)
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

void FollowRequestModel::actionDeny(const QModelIndex &index)
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

bool FollowRequestModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return !m_next.isEmpty();
}

void FollowRequestModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    fillTimeline();
}

void FollowRequestModel::fillTimeline()
{
    auto account = AccountManager::instance().selectedAccount();

    setLoading(true);

    QUrl url;
    if (m_next.isEmpty()) {
        url = account->apiUrl("/api/v1/follow_requests");
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

            beginResetModel();

            std::transform(
                accounts.cbegin(),
                accounts.cend(),
                std::back_inserter(m_accounts),
                [account](const QJsonValue &value) -> auto{
                    const auto identityJson = value.toObject();
                    return account->identityLookup(identityJson["id"].toString(), identityJson);
                });

            endResetModel();
        }

        setLoading(false);
    });
}
