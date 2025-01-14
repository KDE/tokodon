// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/filtersmodel.h"

#include <QJsonDocument>
#include <QNetworkReply>

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "networkcontroller.h"

using namespace Qt::Literals::StringLiterals;

FiltersModel::FiltersModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fillTimeline();
}

QVariant FiltersModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &filter = m_filters[index.row()];

    switch (role) {
    case IdRole:
        return filter.id;
    case TitleRole:
        return filter.title;
    default:
        return {};
    }
}

bool FiltersModel::loading() const
{
    return m_loading;
}

void FiltersModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int FiltersModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_filters.size();
}

QHash<int, QByteArray> FiltersModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {TitleRole, "title"},
    };
}

void FiltersModel::fillTimeline()
{
    const auto account = AccountManager::instance().selectedAccount();

    if (m_loading) {
        return;
    }
    setLoading(true);

    beginResetModel();
    m_filters.clear();
    endResetModel();

    account->get(
        account->apiUrl(QStringLiteral("/api/v2/filters")),
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());
            auto filters = doc.array().toVariantList();

            if (!filters.isEmpty()) {
                QList<Filter> fetchedLists;

                std::ranges::transform(std::as_const(filters), std::back_inserter(fetchedLists), [this](const QVariant &value) -> auto {
                    return fromSourceData(value.toJsonObject());
                });
                beginInsertRows({}, m_filters.size(), m_filters.size() + fetchedLists.size() - 1);
                m_filters += fetchedLists;
                endInsertRows();
            }

            setLoading(false);
        },
        [=](QNetworkReply *reply) {
            NetworkController::instance().networkErrorOccurred(reply->errorString());
        });
}

FiltersModel::Filter FiltersModel::fromSourceData(const QJsonObject &object) const
{
    Filter filter;
    filter.id = object["id"_L1].toString();
    filter.title = object["title"_L1].toString();
    filter.context = object["context"_L1].toString();
    if (object.contains("created_at"_L1)) {
        filter.expiresAt = QDateTime::fromString(object["created_at"_L1].toString(), Qt::ISODate).toLocalTime();
    }
    filter.filterAction = object["filter_action"_L1].toString();

    return filter;
}

#include "moc_filtersmodel.cpp"
