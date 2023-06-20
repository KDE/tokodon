// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "timelinemodel.h"
#include "account/abstractaccount.h"
#include "accountmodel.h"
#include "poll.h"
#include <KLocalizedString>
#include <QJsonArray>
#include <QJsonObject>
#include <QtMath>
#include <algorithm>

TimelineModel::TimelineModel(QObject *parent)
    : AbstractTimelineModel(parent)
    , m_manager(&AccountManager::instance())
{
}

void TimelineModel::init()
{
    m_manager = &AccountManager::instance();
    m_account = m_manager->selectedAccount();

    if (m_account) {
        connect(m_account, &AbstractAccount::streamingEvent, this, &TimelineModel::handleEvent);
    }

    connect(m_manager, &AccountManager::invalidated, this, [=](AbstractAccount *account) {
        if (m_account == account) {
            qDebug() << "Invalidating account" << account;

            beginResetModel();
            m_timeline.clear();
            endResetModel();

            fillTimeline();
        }
    });

    connect(m_manager, &AccountManager::accountSelected, this, [=](AbstractAccount *account) {
        if (m_account == account) {
            return;
        }

        if (m_account) {
            disconnect(m_account, &AbstractAccount::streamingEvent, this, &TimelineModel::handleEvent);
        }

        m_account = account;

        connect(m_account, &AbstractAccount::streamingEvent, this, &TimelineModel::handleEvent);

        beginResetModel();
        qDeleteAll(m_timeline);
        m_timeline.clear();
        endResetModel();

        setLoading(false);

        Q_EMIT nameChanged();
        fillTimeline();
    });

    fillTimeline();
}

void TimelineModel::fetchedTimeline(const QByteArray &data, bool alwaysAppendToEnd)
{
    QList<Post *> posts;

    const auto doc = QJsonDocument::fromJson(data);

    if (!doc.isArray()) {
        setLoading(false);
        return;
    }

    const auto array = doc.array();

    if (array.isEmpty()) {
        setLoading(false);
        return;
    }

    std::transform(array.cbegin(), array.cend(), std::back_inserter(posts), [this](const QJsonValue &value) -> Post * {
        auto post = new Post(m_account, value.toObject(), this);
        if (!post->hidden()) {
            return post;
        } else {
            return nullptr;
        }
    });

    posts.erase(std::remove_if(posts.begin(),
                               posts.end(),
                               [](Post *post) {
                                   return post == nullptr;
                               }),
                posts.end());

    if (!m_timeline.isEmpty()) {
        if (alwaysAppendToEnd) {
            beginInsertRows({}, m_timeline.size(), m_timeline.size() + posts.size() - 1);
            m_timeline += posts;
            endInsertRows();
        } else {
            const auto postOld = m_timeline.first();
            const auto postNew = posts.first();
            if (postOld->originalPostId() > postNew->originalPostId()) {
                const int row = m_timeline.size();
                const int last = row + posts.size() - 1;
                beginInsertRows({}, row, last);
                m_timeline += posts;
                endInsertRows();
            } else {
                beginInsertRows({}, 0, posts.size() - 1);
                m_timeline = posts + m_timeline;
                endInsertRows();
            }
        }
    } else {
        beginInsertRows({}, 0, posts.size() - 1);
        m_timeline = posts;
        endInsertRows();
    }
    setLoading(false);
}

void TimelineModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (m_timeline.empty()) {
        return;
    }

    const auto p = m_timeline.last();

    fillTimeline(p->originalPostId());
}

bool TimelineModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return true;
}

int TimelineModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_timeline.size();
}

QVariant TimelineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    if (role == TypeRole) {
        return false;
    }
    return postData(m_timeline[index.row()], role);
}

void TimelineModel::actionReply(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_timeline[row];

    Q_EMIT wantReply(m_account, p, index);
}

void TimelineModel::actionFavorite(const QModelIndex &index)
{
    const int row = index.row();
    const auto post = m_timeline[row];
    AbstractTimelineModel::actionFavorite(index, post);
}

void TimelineModel::actionRepeat(const QModelIndex &index)
{
    const int row = index.row();
    const auto post = m_timeline[row];
    AbstractTimelineModel::actionRepeat(index, post);
}

void TimelineModel::actionVote(const QModelIndex &index, const QList<int> &choices)
{
    const int row = index.row();
    const auto post = m_timeline[row];
    const auto poll = post->poll();
    Q_ASSERT(poll);

    QJsonObject obj;
    QJsonArray array;
    std::transform(
        choices.cbegin(),
        choices.cend(),
        std::back_inserter(array),
        [](int choice) -> auto{ return choice; });
    obj["choices"] = array;
    QJsonDocument doc(obj);
    const auto id = poll->id();

    m_account->post(m_account->apiUrl(QString("/api/v1/polls/%1/votes").arg(id)), doc, true, this, [this, id](QNetworkReply *reply) {
        int i = 0;
        for (auto &post : m_timeline) {
            if (post->poll() && post->poll()->id() == id) {
                const auto newPoll = QJsonDocument::fromJson(reply->readAll()).object();
                post->setPollJson(newPoll);
                Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), {PollRole});
                break;
            }
            i++;
        }
    });
}

void TimelineModel::actionBookmark(const QModelIndex &index)
{
    int row = index.row();
    const auto post = m_timeline[row];

    AbstractTimelineModel::actionBookmark(index, post);
}

void TimelineModel::actionPin(const QModelIndex &index)
{
    int row = index.row();
    const auto post = m_timeline[row];

    AbstractTimelineModel::actionPin(index, post);
}

void TimelineModel::actionRedraft(const QModelIndex &index, bool isEdit)
{
    int row = index.row();
    auto p = m_timeline[row];

    AbstractTimelineModel::actionRedraft(index, p, isEdit);
}

void TimelineModel::actionDelete(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_timeline[row];

    AbstractTimelineModel::actionDelete(index, p);

    beginRemoveRows({}, row, row);
    m_timeline.removeAt(row);
    endRemoveRows();
}

void TimelineModel::handleEvent(AbstractAccount::StreamingEventType eventType, const QByteArray &payload)
{
    if (eventType == AbstractAccount::StreamingEventType::DeleteEvent) {
        int i = 0;
        for (const auto &post : std::as_const(m_timeline)) {
            if (post->originalPostId().toUtf8() == payload) {
                beginRemoveRows({}, i, i);
                m_timeline.removeAt(i);
                endRemoveRows();
                break;
            }
            i++;
        }
    }
}