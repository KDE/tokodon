// SPDX-FileCopyrightText: 2023 Shubham Arora <shubhamarora@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "tagsmodel.h"
#include <KLocalizedString>
#include <QJsonArray>
#include <QUrlQuery>
#include <QtMath>
#include <algorithm>

TagsModel::TagsModel(QObject *parent)
    : AbstractListModel(parent)
{
    connect(this, &AbstractListModel::nameChanged, this, &TagsModel::onNameChanged);
}

void TagsModel::onNameChanged()
{
    fillTimeline();
}

QString TagsModel::displayName() const
{
    return i18n("Trending");
}

void TagsModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (shouldLoadMore()) {
        fillTimeline();
    } else {
        setShouldLoadMore(true);
    }
}

bool TagsModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return !m_next.isEmpty();
}

void TagsModel::fillTimeline(const QString &fromId)
{
    Q_UNUSED(fromId);

    if (!account() || loading() || name() != "trending") {
        return;
    }

    QUrlQuery q;
    q.addQueryItem("limit", "20");
    QUrl uri;
    if (m_next.isEmpty()) {
        if (name() == "trending") {
            uri = account()->apiUrl("/api/v1/trends/tags");
            uri.setQuery(q);
        }
    } else {
        uri = m_next;
    }

    setLoading(true);
    account()->get(
        uri,
        false,
        this,
        [this, uri](QNetworkReply *reply) {
            const auto data = reply->readAll();
            const auto doc = QJsonDocument::fromJson(data);
            if (!doc.isArray()) {
                return;
            }
            static QRegularExpression re("<(.*)>; rel=\"next\"");
            const auto next = reply->rawHeader(QByteArrayLiteral("Link"));
            const auto match = re.match(next);
            m_next = QUrl::fromUserInput(match.captured(1));
            const auto values = doc.array();

            QList<Tag> tags;
            std::transform(values.cbegin(), values.cend(), std::back_inserter(tags), [](const QJsonValue &value) {
                return Tag(value.toObject());
            });

            beginInsertRows({}, m_tags.size(), m_tags.size() + tags.size() - 1);
            m_tags += tags;
            endInsertRows();
            setLoading(false);
        },
        [this](QNetworkReply *reply) {
            Q_UNUSED(reply);
            setLoading(false);
        });
}

QHash<int, QByteArray> TagsModel::roleNames() const
{
    return {
        {CustomRoles::NameRole, "name"},
        {CustomRoles::UrlRole, "url"},
        {CustomRoles::HistoryRole, "history"},
    };
}

QVariant TagsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    switch (role) {
    case CustomRoles::NameRole:
        return m_tags[index.row()].name();
    case CustomRoles::UrlRole:
        return m_tags[index.row()].url();
    case CustomRoles::HistoryRole:
        return QVariant::fromValue<QList<History>>(m_tags[index.row()].history());
    default:
        return {};
    }
}

int TagsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_tags.size();
}
