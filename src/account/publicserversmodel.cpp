// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/publicserversmodel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

using namespace Qt::StringLiterals;

PublicServersModel::PublicServersModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fill();
}

QVariant PublicServersModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &instance = m_servers[index.row()];

    switch (role) {
    case DomainRole:
        return instance.domain;
    case DescriptionRole:
        return instance.description;
    case IconRole:
        return instance.thumbnailUrl;
    case IsPublicRole:
        return true;
    default:
        return {};
    }
}

int PublicServersModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_servers.size();
}

QHash<int, QByteArray> PublicServersModel::roleNames() const
{
    return {{DomainRole, "domain"}, {DescriptionRole, "description"}, {IconRole, "iconSource"}, {IsPublicRole, "isPublic"}};
}

bool PublicServersModel::loading() const
{
    return m_loading;
}

void PublicServersModel::fill()
{
    if (loading()) {
        return;
    }
    setLoading(true);

    auto reply = m_netManager.get(QNetworkRequest(QUrl(QStringLiteral("https://api.joinmastodon.org/servers"))));
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        auto instances = doc.array().toVariantList();

        if (!instances.isEmpty()) {
            QList<PublicServer> fetchedInstances;

            std::ranges::transform(std::as_const(instances), std::back_inserter(fetchedInstances), [this](const QVariant &value) -> auto {
                return fromSourceData(value.toJsonObject());
            });
            beginInsertRows({}, m_servers.size(), m_servers.size() + fetchedInstances.size() - 1);
            m_servers += fetchedInstances;
            endInsertRows();
        }

        setLoading(false);
    });
}

void PublicServersModel::setLoading(const bool loading)
{
    if (loading != m_loading) {
        m_loading = loading;
        Q_EMIT loadingChanged();
    }
}

PublicServersModel::PublicServer PublicServersModel::fromSourceData(const QJsonObject &obj) const
{
    PublicServer instance;
    instance.domain = obj[u"domain"_s].toString();
    instance.description = obj[u"description"_s].toString();
    instance.thumbnailUrl = obj[u"proxied_thumbnail"_s].toString();

    return instance;
}

#include "moc_publicserversmodel.cpp"
