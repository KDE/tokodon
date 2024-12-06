// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/announcementmodel.h"

#include <QJsonDocument>
#include <QNetworkReply>

#include "account/accountmanager.h"
#include "networkcontroller.h"

using namespace Qt::Literals::StringLiterals;

AnnouncementModel::AnnouncementModel(QObject *parent)
    : QAbstractListModel(parent)
{
    fillTimeline();
}

QVariant AnnouncementModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &announcement = m_announcements[index.row()];

    switch (role) {
    case IdRole:
        return announcement.id;
    case ContentRole:
        return announcement.content;
    case PublishedAt:
        return announcement.publishedAt;
    default:
        return {};
    }
}

bool AnnouncementModel::loading() const
{
    return m_loading;
}

void AnnouncementModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

int AnnouncementModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_announcements.size();
}

QHash<int, QByteArray> AnnouncementModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {ContentRole, "content"},
        {PublishedAt, "publishedAt"},
    };
}

void AnnouncementModel::fillTimeline()
{
    const auto account = AccountManager::instance().selectedAccount();

    if (m_loading) {
        return;
    }
    setLoading(true);

    account->get(
        account->apiUrl(QStringLiteral("/api/v1/announcements")),
        true,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());
            auto announcements = doc.array().toVariantList();
            std::ranges::reverse(announcements);

            if (!announcements.isEmpty()) {
                QList<Announcement> fetchedAnnouncements;

                std::ranges::transform(std::as_const(announcements), std::back_inserter(fetchedAnnouncements), [this](const QVariant &value) -> auto {
                    return fromSourceData(value.toJsonObject());
                });
                beginInsertRows({}, m_announcements.size(), m_announcements.size() + fetchedAnnouncements.size() - 1);
                m_announcements += fetchedAnnouncements;
                endInsertRows();
            }

            setLoading(false);
        },
        [=](QNetworkReply *reply) {
            Q_EMIT NetworkController::instance().networkErrorOccurred(reply->errorString());
        });
}

AnnouncementModel::Announcement AnnouncementModel::fromSourceData(const QJsonObject &object) const
{
    Announcement announcement;
    announcement.id = object["id"_L1].toString();
    announcement.content = object["content"_L1].toString();
    announcement.publishedAt = QDateTime::fromString(object["published_at"_L1].toString(), Qt::ISODate).toLocalTime();

    return announcement;
}

#include "moc_announcementmodel.cpp"
