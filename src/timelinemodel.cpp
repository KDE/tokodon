// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "timelinemodel.h"
#include "abstracttimelinemodel.h"
#include "accountmodel.h"
#include "threadmodel.h"
#include <KLocalizedString>
#include <QtMath>

TimelineModel::TimelineModel(QObject *parent)
    : AbstractTimelineModel(parent)
    , m_last_fetch(time(nullptr))
{
    connect(m_manager, &AccountManager::accountSelected, this, &TimelineModel::nameChanged);
}

void TimelineModel::setName(const QString &timelineName)
{
    if (timelineName == m_timelineName) {
        return;
    }

    m_timelineName = timelineName;
    Q_EMIT nameChanged();
}

QString TimelineModel::displayName() const
{
    if (m_timelineName == "home") {
        if (m_manager && m_manager->rowCount() > 1) {
            return i18nc("@title", "Home (%1)", m_manager->selectedAccount()->username());
        } else {
            return i18nc("@title", "Home");
        }
    } else if (m_timelineName == "public") {
        return i18nc("@title", "Local Timeline");
    } else if (m_timelineName == "federated") {
        return i18nc("@title", "Global Timeline");
    }
    return QString();
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

    QObject::connect(m_manager, &AccountManager::accountSelected, this, [=](Account *account) {
        if (m_account == account) {
            return;
        }
        m_account = account;

        beginResetModel();
        m_timeline.clear();
        endResetModel();

        fillTimeline();
    });
    QObject::connect(m_manager, &AccountManager::fetchedTimeline, this, &TimelineModel::fetchedTimeline);
    QObject::connect(m_manager, &AccountManager::invalidated, this, [=](Account *account) {
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
    return m_timelineName;
}

void TimelineModel::fillTimeline(const QString &from_id)
{
    if (m_timelineName != "home" && m_timelineName != "public" && m_timelineName != "federated") {
        return;
    }

    m_fetching = true;

    if (m_account) {
        m_account->fetchTimeline(m_timelineName, from_id);
    }
}

void TimelineModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (m_timeline.size() < 1)
        return;

    auto p = m_timeline.last();

    fillTimeline(p->m_post_id);
}

bool TimelineModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_fetching)
        return false;

    if (time(nullptr) <= m_last_fetch)
        return false;

    return true;
}

void TimelineModel::fetchedTimeline(Account *account, const QString &original_name, const QList<std::shared_ptr<Post>> &posts)
{
    m_fetching = false;

    // make sure the timeline update is for us
    if (account != m_account || original_name != m_timelineName) {
        return;
    }

    if (posts.isEmpty()) {
        return;
    }

    int row = 0;
    int last = 0;

    if (!m_timeline.isEmpty()) {
        const auto post_old = m_timeline.first();
        const auto post_new = posts.first();

        qDebug() << "fetchedTimeline"
                 << "post_old->m_post_id" << post_old->m_post_id << "post_new->m_post_id" << post_new->m_post_id;
        if (post_old->m_post_id > post_new->m_post_id) {
            row = m_timeline.size();
            last = row + posts.size() - 1;
            m_timeline += posts;
        } else {
            row = 0;
            last = posts.size();
            m_timeline = posts + m_timeline;
        }
    } else {
        row = 0;
        last = posts.size() - 1;
        m_timeline = posts;
    }

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
        {AvatarRole, QByteArrayLiteral("avatar")},
        {AuthorDisplayNameRole, QByteArrayLiteral("authorDisplayName")},
        {PinnedRole, QByteArrayLiteral("pinned")},
        {IdRole, QByteArrayLiteral("id")},
        {AuthorIdRole, QByteArrayLiteral("authorId")},
        {PublishedAtRole, QByteArrayLiteral("publishedAt")},
        {MentionsRole, QByteArrayLiteral("mentions")},
        {RelativeTimeRole, QByteArrayLiteral("relativeTime")},
        {SensitiveRole, QByteArrayLiteral("sensitive")},
        {SpoilerTextRole, QByteArrayLiteral("spoilerText")},
        {RebloggedRole, QByteArrayLiteral("reblogged")},
        {WasRebloggedRole, QByteArrayLiteral("wasReblogged")},
        {RebloggedDisplayNameRole, QByteArrayLiteral("rebloggedDisplayName")},
        {RebloggedIdRole, QByteArrayLiteral("rebloggedId")},
        {AttachmentsRole, QByteArrayLiteral("attachments")},
        {ReblogsCountRole, QByteArrayLiteral("reblogsCount")},
        {RepliesCountRole, QByteArrayLiteral("repliesCount")},
        {FavoritedRole, QByteArrayLiteral("favorite")},
        {FavoritesCountRole, QByteArrayLiteral("favoritesCount")},
        {UrlRole, QByteArrayLiteral("url")},
        {ThreadModelRole, QByteArrayLiteral("threadModel")},
        {AccountModelRole, QByteArrayLiteral("accountModel")},
        {CardRole, QByteArrayLiteral("card")},
        {TypeRole, QByteArrayLiteral("type")},
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
    case TypeRole:
        return false;
    case IdRole:
        return p->m_post_id;
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
    case WasRebloggedRole:
        return p->m_repeat;
    case MentionsRole:
        return p->mentions();
    case RebloggedDisplayNameRole:
        if (p->m_repeat_identity) {
            return p->m_repeat_identity->m_display_name;
        }
        return {};
    case RebloggedIdRole:
        if (p->m_repeat_identity) {
            return p->m_repeat_identity->m_acct;
        }
        return {};
    case RebloggedRole:
        return p->m_isRepeated;
    case ReblogsCountRole:
        return p->m_repeatedCount;
    case FavoritedRole:
        return p->m_isFavorite;
    case FavoritesCountRole:
        return p->m_favoriteCount;
    case PinnedRole:
        return p->m_pinned;
    case SensitiveRole:
        return p->m_isSensitive;
    case RepliesCountRole:
        return p->m_repliesCount;
    case SpoilerTextRole:
        return p->m_subject;
    case AttachmentsRole:
        return QVariant::fromValue<QList<Attachment *>>(p->m_attachments);
    case CardRole:
        if (p->card().has_value()) {
            return QVariant::fromValue<Card>(*p->card());
        }
        return false;
    case ThreadModelRole:
        return QVariant::fromValue<QAbstractListModel *>(new ThreadModel(m_manager, p->m_post_id));
    case AccountModelRole:
        return QVariant::fromValue<QAbstractListModel *>(new AccountModel(m_manager, p->m_author_identity->m_id, p->m_author_identity->m_acct));
    case RelativeTimeRole: {
        const auto current = QDateTime::currentDateTime();
        auto secsTo = p->m_published_at.secsTo(current);
        if (secsTo < 60 * 60) {
            const auto hours = p->m_published_at.time().hour();
            const auto minutes = p->m_published_at.time().minute();
            return i18nc("hour:minute",
                         "%1:%2",
                         hours < 10 ? QChar('0') + QString::number(hours) : QString::number(hours),
                         minutes < 10 ? QChar('0') + QString::number(minutes) : QString::number(minutes));
        } else if (secsTo < 60 * 60 * 24) {
            return i18n("%1h", qCeil(secsTo / (60 * 60)));
        } else if (secsTo < 60 * 60 * 24 * 7) {
            return i18n("%1d", qCeil(secsTo / (60 * 60 * 24)));
        }
        return QLocale::system().toString(p->m_published_at.date(), QLocale::ShortFormat);
    }
    }

    return {};
}

void TimelineModel::actionReply(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_timeline[row];

    Q_EMIT wantReply(m_account, p, index);
}

void TimelineModel::actionMenu(const QModelIndex &index)
{
    int row = index.row();
    auto p = m_timeline[row];

    Q_EMIT wantMenu(m_account, p, index);
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
