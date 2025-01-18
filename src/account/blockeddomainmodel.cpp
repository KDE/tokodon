// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/blockeddomainmodel.h"

#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>

#include "account/abstractaccount.h"
#include "account/accountmanager.h"
#include "networkcontroller.h"

using namespace Qt::Literals::StringLiterals;

BlockedDomainModel::BlockedDomainModel(QObject *parent)
    : QAbstractListModel(parent)
{
    loadDomains();
}

QVariant BlockedDomainModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &filter = m_domains[index.row()];

    switch (role) {
    case Qt::DisplayRole:
    case DomainRole:
        return filter;
    default:
        return {};
    }
}

bool BlockedDomainModel::loading() const
{
    return m_loading;
}

void BlockedDomainModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int BlockedDomainModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_domains.size();
}

QHash<int, QByteArray> BlockedDomainModel::roleNames() const
{
    return {
        {DomainRole, "domain"},
    };
}

void BlockedDomainModel::loadDomains()
{
    if (m_loading) {
        return;
    }
    setLoading(true);

    beginResetModel();
    m_domains.clear();
    endResetModel();

    const auto account = AccountManager::instance().selectedAccount();

    account->get(
        account->apiUrl(QStringLiteral("/api/v1/domain_blocks")),
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());
            const auto filters = doc.array().toVariantList();

            if (!filters.isEmpty()) {
                QList<QString> fetchedLists;

                std::ranges::transform(filters, std::back_inserter(fetchedLists), [](const QVariant &value) -> auto {
                    return value.toString();
                });
                beginInsertRows({}, m_domains.size(), m_domains.size() + fetchedLists.size() - 1);
                m_domains += fetchedLists;
                endInsertRows();
            }

            setLoading(false);
        },
        [=](QNetworkReply *reply) {
            NetworkController::instance().networkErrorOccurred(reply->errorString());
        });
}

void BlockedDomainModel::actionUnblock(const QModelIndex &index)
{
    auto account = AccountManager::instance().selectedAccount();
    const QUrlQuery query{{{QStringLiteral("domain"), m_domains.at(index.row())}}};
    QUrl url = account->apiUrl(QStringLiteral("/api/v1/domain_blocks"));
    url.setQuery(query);
    account->deleteResource(url, true, this, [this, index](QNetworkReply *reply) {
        Q_UNUSED(reply)
        beginRemoveRows({}, index.row(), index.row());
        m_domains.removeAt(index.row());
        endRemoveRows();
    });
}

#include "moc_blockeddomainmodel.cpp"
