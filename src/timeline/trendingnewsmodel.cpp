// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "timeline/trendingnewsmodel.h"

#include "networkcontroller.h"

#include <KLocalizedString>

using namespace Qt::StringLiterals;

TrendingNewsModel::TrendingNewsModel(QObject *parent)
    : AbstractListModel(parent)
{
    fill();
}

TrendingNewsModel::~TrendingNewsModel() = default;

QVariant TrendingNewsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &link = m_links[index.row()];

    switch (role) {
    case UrlRole:
        return link.url;
    case TitleRole:
        return link.title;
    case DescriptionRole:
        return link.description;
    case ImageRole:
        return link.image;
    default:
        return {};
    }
}

int TrendingNewsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_links.size();
}

QHash<int, QByteArray> TrendingNewsModel::roleNames() const
{
    return {{UrlRole, "url"}, {TitleRole, "title"}, {DescriptionRole, "description"}, {ImageRole, "image"}};
}

QString TrendingNewsModel::displayName() const
{
    return i18nc("@title:window", "Trending News");
}

void TrendingNewsModel::fill()
{
    if (loading()) {
        return;
    }
    setLoading(true);

    account()->get(
        account()->apiUrl(QStringLiteral("/api/v1/trends/links")),
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());
            auto links = doc.array().toVariantList();
            std::reverse(links.begin(), links.end());

            if (!links.isEmpty()) {
                QList<Link> fetchedLinks;

                std::transform(links.cbegin(), links.cend(), std::back_inserter(fetchedLinks), [=](const QVariant &value) -> auto {
                    return fromSourceData(value.toJsonObject());
                });
                beginInsertRows({}, m_links.size(), m_links.size() + fetchedLinks.size() - 1);
                m_links += fetchedLinks;
                endInsertRows();
            }

            setLoading(false);
        },
        [=](QNetworkReply *reply) {
            setLoading(false);
            Q_EMIT NetworkController::instance().networkErrorOccurred(reply->errorString());
        });
}

TrendingNewsModel::Link TrendingNewsModel::fromSourceData(const QJsonObject &object) const
{
    Link link;
    link.url = object["url"_L1].toString();
    link.title = object["title"_L1].toString();
    link.description = object["description"_L1].toString();
    link.image = object["image"_L1].toString();

    return link;
}

#include "moc_trendingnewsmodel.cpp"
