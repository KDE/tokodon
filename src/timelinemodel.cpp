// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "timelinemodel.h"

TimelineModel::TimelineModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void TimelineModel::setName(const QString &timeline_name)
{
    if (timeline_name == m_timeline_name || (timeline_name != "home" && timeline_name != "public" && timeline_name != "federated")) {
        return;
    }

    m_timeline_name = timeline_name;
    Q_EMIT nameChanged();
    fillTimeline();
}

void TimelineModel::setAccountManager(AccountManager *accountManager)
{
    if (accountManager == m_manager) {
        return;
    }

    if (m_manager) {
        disconnect(m_manager, nullptr, this, nullptr);
    }

    m_manager = accountManager;
    m_account = m_manager->selectedAccount();

    Q_EMIT accountManagerChanged();

    QObject::connect(m_manager, &AccountManager::accountSelected, this, [=] (Account *account) {
        m_account = account;

        beginResetModel();
        m_timeline.clear();
        endResetModel();

        fillTimeline();
    });
    QObject::connect(m_manager, &AccountManager::fetchedTimeline, this, &TimelineModel::fetchedTimeline);
    QObject::connect(m_manager, &AccountManager::invalidated, this, [=] (Account *account) {
        if (m_account == account) {
            qDebug() << "Invalidating account" << account;

            beginResetModel();
            m_timeline.clear();
            endResetModel();

            fillTimeline();
        }
    });

    fillTimeline();
}

AccountManager *TimelineModel::accountManager() const
{
    return m_manager;
}

QString TimelineModel::name() const
{
    return m_timeline_name;
}

void TimelineModel::fillTimeline(QString from_id)
{
    if (m_timeline_name != "home" && m_timeline_name != "public" && m_timeline_name != "federated") {
        return;
    }

    m_fetching = true;

    if (m_account)
        m_account->fetchTimeline(m_timeline_name, from_id);
}

void TimelineModel::fetchMore(const QModelIndex &parent)
{
    (void) parent;

    if (m_timeline.size() < 1)
        return;

    auto p = m_timeline.last();

    fillTimeline(p->m_post_id);
}

bool TimelineModel::canFetchMore(const QModelIndex &parent) const
{
    (void) parent;

    if (m_fetching)
        return false;

    if (time(nullptr) <= m_last_fetch)
        return false;

    return true;
}

void TimelineModel::fetchedTimeline(Account *account, QString original_name, QList<std::shared_ptr<Post>> posts)
{
    m_fetching = false;

    // make sure the timeline update is for us
    if (account != m_account || original_name != m_timeline_name)
        return;

    if (posts.isEmpty())
        return;

    int row, last;

    if (! m_timeline.isEmpty())
    {
        auto post_old = m_timeline.first();
        auto post_new = posts.first();

        if (post_old->m_post_id > post_new->m_post_id)
        {
            row = m_timeline.size();
            last = row + posts.size() - 1;
            m_timeline += posts;
        }
        else
        {
            row = 0;
            last = posts.size();
            m_timeline = posts + m_timeline;
        }
    }
    else
    {
        row = 0;
        last = posts.size();
        m_timeline = posts;
    }

    for (auto p : m_timeline)
        p->fetchAttachmentPreviews();

    beginInsertRows(QModelIndex(), row, last);
    endInsertRows();

    m_last_fetch = time(nullptr);
}

int TimelineModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_timeline.size();
}

// this is even more extremely cursed
std::shared_ptr<Post> TimelineModel::internalData(const QModelIndex &index) const
{
    int row = index.row();
    return m_timeline[row];
}

QHash<int, QByteArray> TimelineModel::roleNames() const
{
    return {
        {Qt::DisplayRole, QByteArrayLiteral("display")},
        {Qt::DecorationRole, QByteArrayLiteral("avatar")},
        {AuthorDisplayNameRole, QByteArrayLiteral("authorDisplayName")},
        {AuthorIdRole, QByteArrayLiteral("authorId")},
        {PublishedAtRole, QByteArrayLiteral("publishedAt")}
    };
}

QVariant TimelineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    int row = index.row();
    auto p = m_timeline[row];

    switch (role) {
    case Qt::DisplayRole:
        return p->m_content;
    case AvatarRole:
        return p->m_author_identity->m_avatarUrl;
    case AuthorDisplayNameRole:
        return p->m_author_identity->m_display_name;
    case AuthorIdRole:
        return p->m_author_identity->m_acct;
    case PublishedAtRole:
        return p->m_published_at;

    }

    return {};
}

void TimelineModel::actionReply(const QModelIndex &index)
{
    int row = index.row ();
    auto p = m_timeline[row];

    Q_EMIT wantReply (m_account, p, index);
}

void TimelineModel::actionMenu(const QModelIndex &index)
{
    int row = index.row ();
    auto p = m_timeline[row];

    Q_EMIT wantMenu (m_account, p, index);
}

void TimelineModel::actionFavorite(const QModelIndex &index)
{
    int row = index.row ();
    auto p = m_timeline[row];

    if (! p->m_is_favorited)
    {
        m_account->favorite (p);
        p->m_is_favorited = true;
    }
    else
    {
        m_account->unfavorite (p);
        p->m_is_favorited = false;
    }

    Q_EMIT dataChanged(index,index);
}

void TimelineModel::actionRepeat(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_timeline[row];

    if (! p->m_is_repeated)
    {
        m_account->repeat (p);
        p->m_is_repeated = true;
    }
    else
    {
        m_account->unrepeat (p);
        p->m_is_repeated = false;
    }

    Q_EMIT dataChanged(index, index);
}

void TimelineModel::actionVis(const QModelIndex &index)
{
    int row = index.row ();
    auto p = m_timeline[row];

    p->m_attachments_visible ^= true;

    Q_EMIT dataChanged(index, index);
}

void TimelineModel::actionExpand(const QModelIndex &index)
{
    int row = index.row ();
    auto p = m_timeline[row];

    p->m_is_expanded ^= true;

    Q_EMIT dataChanged(index, index);
}

void TimelineModel::showThreadWindow(const QModelIndex &index)
{
    /*int row = index.row ();
    auto p = m_timeline[row];

    auto win = new ThreadWindow(m_manager, p->m_post_id);
    win->show();*/
}

void TimelineModel::handleMouseOver(const QModelIndex &index)
{
    Q_EMIT dataChanged(index, index);
}
