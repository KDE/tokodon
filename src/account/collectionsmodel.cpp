// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/collectionsmodel.h"

#include <QJsonDocument>
#include <QNetworkReply>

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "networkcontroller.h"

using namespace Qt::Literals::StringLiterals;

CollectionsModel::CollectionsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fillTimeline();
}

QVariant CollectionsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &collection = m_collections[index.row()];

    switch (role) {
    case IdRole:
        return collection.id;
    case NameRole:
        return collection.name;
    default:
        return {};
    }
}

bool CollectionsModel::loading() const
{
    return m_loading;
}

void CollectionsModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int CollectionsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_collections.size();
}

QHash<int, QByteArray> CollectionsModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {NameRole, "name"},
    };
}

void CollectionsModel::fillTimeline()
{
    const auto account = AccountManager::instance().selectedAccount();

    if (m_loading) {
        return;
    }
    setLoading(true);

    beginResetModel();
    m_collections.clear();
    endResetModel();

    account->get(
        account->apiUrl(QStringLiteral("/api/v1/accounts/%1/collections").arg(account->identity()->id())),
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());
            auto lists = doc["collections"_L1].toArray().toVariantList();

            if (!lists.isEmpty()) {
                QList<Collection> fetchedCollections;

                std::ranges::transform(std::as_const(lists), std::back_inserter(fetchedCollections), [this](const QVariant &value) -> auto {
                    return fromSourceData(value.toJsonObject());
                });
                beginInsertRows({}, m_collections.size(), m_collections.size() + fetchedCollections.size() - 1);
                m_collections += fetchedCollections;
                endInsertRows();
            }

            setLoading(false);
        },
        [=](QNetworkReply *reply) {
            NetworkController::instance().networkErrorOccurred(reply->errorString());
        });
}

CollectionsModel::Collection CollectionsModel::fromSourceData(const QJsonObject &object) const
{
    Collection collection;
    collection.id = object["id"_L1].toString();
    collection.name = object["name"_L1].toString();

    return collection;
}

#include "moc_collectionsmodel.cpp"
