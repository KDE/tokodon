// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstracttimelinemodel.h"
#include "account.h"
#include "attachmenteditormodel.h"
#include "identity.h"
#include "poll.h"
#include "post.h"
#include "posteditorbackend.h"
#include <KLocalizedString>
#include <QtMath>
#include <qvariant.h>

AbstractTimelineModel::AbstractTimelineModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

bool AbstractTimelineModel::loading() const
{
    return m_loading;
}

void AbstractTimelineModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

QHash<int, QByteArray> AbstractTimelineModel::roleNames() const
{
    return {
        {IdRole, QByteArrayLiteral("id")},
        {UrlRole, QByteArrayLiteral("url")},
        {ContentRole, QByteArrayLiteral("content")},
        {SpoilerTextRole, QByteArrayLiteral("spoilerText")},
        {AuthorIdentityRole, QByteArrayLiteral("authorIdentity")},
        {PublishedAtRole, QByteArrayLiteral("publishedAt")},
        {VisibilityRole, QByteArrayLiteral("visibility")},
        {SelectedRole, QByteArrayLiteral("selected")},
        {FiltersRole, QByteArrayLiteral("filters")},
        {RelativeTimeRole, QByteArrayLiteral("relativeTime")},
        {SensitiveRole, QByteArrayLiteral("sensitive")},

        // Additional content
        {CardRole, QByteArrayLiteral("card")},
        {PollRole, QByteArrayLiteral("poll")},
        {MentionsRole, QByteArrayLiteral("mentions")},
        {AttachmentsRole, QByteArrayLiteral("attachments")},

        // Reblog
        {IsBoostedRole, "isBoosted"},
        {BoostAuthorIdentityRole, "boostAuthorIdentity"},

        // Interaction count
        {ReblogsCountRole, QByteArrayLiteral("reblogsCount")},
        {RepliesCountRole, QByteArrayLiteral("repliesCount")},
        {FavouritesCountRole, QByteArrayLiteral("favouritesCount")},

        // User self interaction
        {FavouritedRole, QByteArrayLiteral("favourited")},
        {RebloggedRole, QByteArrayLiteral("reblogged")},
        {MutedRole, QByteArrayLiteral("muted")},
        {BookmarkedRole, QByteArrayLiteral("bookmarked")},
        {PinnedRole, QByteArrayLiteral("pinned")},

        // Notification
        {NotificationActorIdentityRole, "notificationActorIdentity"},
        {TypeRole, "type"},
    };
}

QVariant AbstractTimelineModel::postData(Post *post, int role) const
{
    switch (role) {
    case IdRole:
        return post->postId();
    case MentionsRole:
        return post->mentions();
    case ContentRole:
        return post->content();
    case AuthorIdentityRole:
        return QVariant::fromValue<Identity *>(post->authorIdentity().get());
    case IsBoostedRole:
        return post->boosted();
    case BoostAuthorIdentityRole:
        if (post->boostIdentity()) {
            return QVariant::fromValue<Identity *>(post->boostIdentity().get());
        }
        return false;
    case PublishedAtRole:
        return post->publishedAt();
    case RebloggedRole:
        return post->reblogged();
    case FavouritedRole:
        return post->favourited();
    case BookmarkedRole:
        return post->bookmarked();
    case PinnedRole:
        return post->pinned();

    case FavouritesCountRole:
        return post->favouritesCount();
    case RepliesCountRole:
        return post->repliesCount();
    case ReblogsCountRole:
        return post->reblogsCount();
    case SensitiveRole:
        return post->sensitive();
    case SpoilerTextRole:
        return post->spoilerText();
    case VisibilityRole:
        return post->visibility();
    case FiltersRole:
        return post->filters();
    case AttachmentsRole:
        return QVariant::fromValue<QVector<Attachment *>>(post->attachments());
    case CardRole:
        if (post->card().has_value()) {
            return QVariant::fromValue<Card>(*post->card());
        }
        return false;
    case UrlRole:
        return QVariant::fromValue<QUrl>(post->url());
    case RelativeTimeRole: {
        const auto current = QDateTime::currentDateTime();
        auto secsTo = post->publishedAt().secsTo(current);
        if (secsTo < 60 * 60) {
            const auto hours = post->publishedAt().time().hour();
            const auto minutes = post->publishedAt().time().minute();
            return i18nc("hour:minute",
                         "%1:%2",
                         hours < 10 ? QChar('0') + QString::number(hours) : QString::number(hours),
                         minutes < 10 ? QChar('0') + QString::number(minutes) : QString::number(minutes));
        } else if (secsTo < 60 * 60 * 24) {
            return i18n("%1h", qCeil(secsTo / (60 * 60)));
        } else if (secsTo < 60 * 60 * 24 * 7) {
            return i18n("%1d", qCeil(secsTo / (60 * 60 * 24)));
        }
        return QLocale::system().toString(post->publishedAt().date(), QLocale::ShortFormat);
    }
    case PollRole:
        if (post->poll()) {
            return QVariant::fromValue<Poll>(*post->poll());
        }
        return {};
    case TypeRole:
    case NotificationActorIdentityRole:
        return false;
    }

    return {};
}

void AbstractTimelineModel::actionFavorite(const QModelIndex &index, Post *post)
{
    if (!post->favourited()) {
        m_account->favorite(post);
        post->setFavourited(true);
    } else {
        m_account->unfavorite(post);
        post->setFavourited(false);
    }

    Q_EMIT dataChanged(index, index);
}

void AbstractTimelineModel::actionRepeat(const QModelIndex &index, Post *post)
{
    if (!post->reblogged()) {
        m_account->repeat(post);
        post->setReblogged(true);
    } else {
        m_account->unrepeat(post);
        post->setReblogged(false);
    }

    Q_EMIT dataChanged(index, index);
}

void AbstractTimelineModel::actionRedraft(const QModelIndex &index, Post *post, bool isEdit)
{
    m_account->get(m_account->apiUrl(QString("/api/v1/statuses/%1/source").arg(post->postId())), true, this, [this, post, index, isEdit](QNetworkReply *reply) {
        const auto postSource = QJsonDocument::fromJson(reply->readAll()).object();

        auto backend = new PostEditorBackend();
        backend->setId(post->postId());
        backend->setStatus(postSource["text"].toString());
        backend->setSpoilerText(postSource["spoiler_text"].toString());
        backend->setInReplyTo(post->inReplyTo());
        backend->setVisibility(post->visibility());
        backend->setLanguage(post->language());
        backend->setMentions(post->mentions()); // TODO: needed?
        backend->setSensitive(post->sensitive());

        Q_EMIT postSourceReady(backend, isEdit);

        auto attachmentBackend = backend->attachmentEditorModel();
        for (const auto &attachment : post->attachments()) {
            attachmentBackend->appendExisting(attachment);
        }

        if (isEdit) {
            connect(backend, &PostEditorBackend::editComplete, this, [this, post, index](QJsonObject object) {
                post->fromJson(object);
                Q_EMIT dataChanged(index, index);
            });
        }
    });
}

void AbstractTimelineModel::actionDelete(const QModelIndex &index, Post *post)
{
    m_account->deleteResource(m_account->apiUrl(QString("/api/v1/statuses/%1").arg(post->postId())), true, this, [this, post, index](QNetworkReply *reply) {
        const auto postSource = QJsonDocument::fromJson(reply->readAll()).object();
        qDebug() << "DELETED: " << postSource;
    });
}