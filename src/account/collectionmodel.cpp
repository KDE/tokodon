// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/collectionmodel.h"

#include "accountmanager.h"
#include "networkcontroller.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>

using namespace Qt::Literals::StringLiterals;

CollectionModel::CollectionModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_account(AccountManager::instance().selectedAccount())
{
}

QVariant CollectionModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    switch (role) {
    case IdentityRole:
        return QVariant::fromValue(m_account->identityLookup(m_items[index.row()].accountId, {}).get());
    default:
        return {};
    }
}

bool CollectionModel::loading() const
{
    return m_loading;
}

void CollectionModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

QString CollectionModel::collectionId() const
{
    return m_collectionId;
}

void CollectionModel::setCollectionId(const QString &collectionId)
{
    if (m_collectionId == collectionId) {
        return;
    }
    m_collectionId = collectionId;
    Q_EMIT collectionIdChanged();

    fill();
}

QString CollectionModel::name() const
{
    return m_name;
}

QString CollectionModel::description() const
{
    return m_description;
}

QString CollectionModel::accountId() const
{
    return m_accountId;
}

int CollectionModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.size();
}

QHash<int, QByteArray> CollectionModel::roleNames() const
{
    return {{IdentityRole, "identity"}};
}

void CollectionModel::removeItem(const QModelIndex index)
{
    auto account = AccountManager::instance().selectedAccount();

    if (!checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid) || m_collectionId.isEmpty()) {
        return;
    }

    auto url = account->apiUrl(QStringLiteral("/api/v1/collections/%1/items/%2").arg(m_collectionId).arg(m_items.at(index.row()).id));

    account->deleteResource(url, true, this, [this, index](QNetworkReply *reply) {
        Q_UNUSED(reply)

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        m_items.removeAt(index.row());
        endRemoveRows();
    });
}

void CollectionModel::fill()
{
    if (m_loading) {
        return;
    }
    setLoading(true);

    if (m_collectionId.isEmpty()) {
        return;
    }

    // TODO: if v2, use the rules from the fetched metadata
    m_account->get(m_account->apiUrl(QStringLiteral("/api/v1/collections/%1").arg(m_collectionId)), false, this, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());

        const auto collectionObj = doc["collection"_L1].toObject();

        m_name = collectionObj["name"_L1].toString();
        Q_EMIT nameChanged();

        m_description = collectionObj["description"_L1].toString();
        Q_EMIT descriptionChanged();

        m_accountId = collectionObj["account_id"_L1].toString();
        Q_EMIT accountIdChanged();

        const auto itemsArray = collectionObj["items"_L1].toArray();
        const auto accountsArray = doc["accounts"_L1].toArray();
        for (auto &item : itemsArray) {
            // Add identity if found.
            for (const auto &account : accountsArray) {
                if (account["id"_L1] == item.toObject()["account_id"_L1].toString()) {
                    Q_UNUSED(m_account->identityLookup(item.toObject()["account_id"_L1].toString(), account.toObject()));

                    break;
                }
            }

            beginInsertRows({}, m_items.size(), m_items.size());
            // TODO: maybe show other states?
            if (item.toObject()["state"_L1] == "accepted"_L1) {
                m_items.push_back({
                    .id = item.toObject()["id"_L1].toString(),
                    .accountId = item.toObject()["account_id"_L1].toString(),
                });
            }
            endInsertRows();
        }

        setLoading(false);
    });
}

#include "moc_collectionmodel.cpp"
