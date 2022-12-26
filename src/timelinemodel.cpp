// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "timelinemodel.h"
#include "abstractaccount.h"
#include "accountmodel.h"
#include "identity.h"
#include "poll.h"
#include "threadmodel.h"
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
        if (m_account != account) {
            m_account = account;

            beginResetModel();
            qDeleteAll(m_timeline);
            m_timeline.clear();
            endResetModel();

            m_loading = false;
            Q_EMIT loadingChanged();

            Q_EMIT nameChanged();
            fillTimeline();
        }
    });

    fillTimeline();
}

void TimelineModel::fetchedTimeline(const QByteArray &data)
{
    QList<Post *> posts;

    const auto doc = QJsonDocument::fromJson(data);

    if (!doc.isArray()) {
        setLoading(false);
        return;
    }

    const auto array = doc.array();
    std::transform(array.cbegin(), array.cend(), std::back_inserter(posts), [this](const QJsonValue &value) {
        return new Post(m_account, value.toObject(), this);
    });

    if (!m_timeline.isEmpty()) {
        const auto postOld = m_timeline.first();
        const auto postNew = posts.first();
        if (postOld->postId() > postNew->postId()) {
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

    if (m_timeline.size() < 1) {
        return;
    }

    const auto p = m_timeline.last();

    fillTimeline(p->m_original_post_id);
}

bool TimelineModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return !m_loading;
}

int TimelineModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_timeline.size();
}

// this is even more extremely cursed
Post *TimelineModel::internalData(const QModelIndex &index) const
{
    int row = index.row();
    return m_timeline[row];
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
    int row = index.row();
    auto p = m_timeline[row];

    if (!p->m_isFavorite) {
        m_account->favorite(p);
        p->m_isFavorite = true;
    } else {
        m_account->unfavorite(p);
        p->m_isFavorite = false;
    }

    Q_EMIT dataChanged(index, index);
}

void TimelineModel::actionRepeat(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_timeline[row];

    if (!p->m_isRepeated) {
        m_account->repeat(p);
        p->m_isRepeated = true;
    } else {
        m_account->unrepeat(p);
        p->m_isRepeated = false;
    }

    Q_EMIT dataChanged(index, index);
}

void TimelineModel::actionVis(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_timeline[row];

    p->m_attachments_visible ^= true;

    Q_EMIT dataChanged(index, index);
}

void TimelineModel::actionVote(const QModelIndex &index, const QList<int> &choices)
{
    int row = index.row();
    auto post = m_timeline[row];
    auto poll = post->poll();
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
                delete post->poll();
                post->setPoll(new Poll(newPoll));
                Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), {PollRole});
                break;
            }
            i++;
        }
    });
}

void TimelineModel::refresh()
{
    fillTimeline();
}
