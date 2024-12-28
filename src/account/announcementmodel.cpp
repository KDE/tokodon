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
    case PublishedAtRole:
        return announcement.publishedAt;
    case ReactionsRole:
        return QVariant::fromValue(announcement.reactions);
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
        {PublishedAtRole, "publishedAt"},
        {ReactionsRole, "reactions"},
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

void AnnouncementModel::addReaction(const QModelIndex index, const QString &name)
{
    const auto announcement = m_announcements.at(index.row());
    const auto account = AccountManager::instance().selectedAccount();
    account->put(account->apiUrl(QStringLiteral("/api/v1/announcements/%1/reactions/%2").arg(announcement.id, name)),
                 QJsonDocument{},
                 true,
                 this,
                 [this, index, name](QNetworkReply *reply) {
                     Q_UNUSED(reply)
                     auto &announcement = m_announcements[index.row()];
                     const auto it = std::ranges::find_if(announcement.reactions, [name](const EmojiReaction &reaction) {
                         return reaction.name == name;
                     });
                     if (it != announcement.reactions.end()) {
                         it->me = true;
                         Q_EMIT dataChanged(AnnouncementModel::index(index.row(), 0), AnnouncementModel::index(index.row(), 0), {ReactionsRole});
                     }
                 });
}

void AnnouncementModel::removeReaction(const QModelIndex index, const QString &name)
{
    const auto announcement = m_announcements.at(index.row());
    const auto account = AccountManager::instance().selectedAccount();
    account->deleteResource(account->apiUrl(QStringLiteral("/api/v1/announcements/%1/reactions/%2").arg(announcement.id, name)),
                            true,
                            this,
                            [this, index, name](QNetworkReply *reply) {
                                Q_UNUSED(reply)
                                auto &announcement = m_announcements[index.row()];
                                const auto it = std::ranges::find_if(announcement.reactions, [name](const EmojiReaction &reaction) {
                                    return reaction.name == name;
                                });
                                if (it != announcement.reactions.end()) {
                                    it->me = false;
                                    Q_EMIT dataChanged(AnnouncementModel::index(index.row(), 0), AnnouncementModel::index(index.row(), 0), {ReactionsRole});
                                }
                            });
}

AnnouncementModel::Announcement AnnouncementModel::fromSourceData(const QJsonObject &object) const
{
    Announcement announcement;
    announcement.id = object["id"_L1].toString();
    announcement.content = object["content"_L1].toString();
    announcement.publishedAt = QDateTime::fromString(object["published_at"_L1].toString(), Qt::ISODate).toLocalTime();

    std::ranges::transform(object["reactions"_L1].toArray(), std::back_inserter(announcement.reactions), [](const QJsonValue &value) {
        EmojiReaction reaction;
        reaction.name = value["name"_L1].toString();
        reaction.count = value["count"_L1].toInt();
        reaction.me = value["me"_L1].toBool();
        reaction.url = value["url"_L1].toString();
        reaction.staticUrl = value["static_url"_L1].toString();

        return reaction;
    });

    return announcement;
}

#include "moc_announcementmodel.cpp"
