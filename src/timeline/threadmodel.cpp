// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "timeline/threadmodel.h"

#include "networkcontroller.h"

#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

ThreadModel::ThreadModel(QObject *parent)
    : TimelineModel(parent)
{
    init();
}

QVariant ThreadModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (role == SelectedRole) {
        return m_postId == TimelineModel::data(index, IdRole).toString();
    } else if (role == IsThreadReplyRole) {
        // This prevents ancestors from being accidentally considered
        const bool isReplyAfterRootPost = index.row() > m_rootPostIndex;
        const bool isReplyToRootPost = m_postId != m_timeline[index.row()]->inReplyTo();

        return isReplyAfterRootPost && isReplyToRootPost;
    } else if (role == IsLastThreadReplyRole) {
        return !data(ThreadModel::index(index.row() + 1, index.column()), IsThreadReplyRole).toBool();
    }

    return TimelineModel::data(index, role);
}

QString ThreadModel::displayName() const
{
    if (m_timeline.isEmpty()) {
        return i18nc("@title:window", "Loading…");
    }
    auto post = m_timeline.at(m_rootPostIndex);

    // FIXME: the inline page title can be HTML, but this is currently synced as the window title. hence why we're not using the HTML version here...
    return i18nc("@title", "Post by %1", post->authorIdentity()->displayName());
}

QString ThreadModel::postId() const
{
    return m_postId;
}

void ThreadModel::setPostId(const QString &postId)
{
    if (m_postId == postId) {
        return;
    }
    m_postId = postId;
    Q_EMIT postIdChanged();

    fillTimeline();
}

QString ThreadModel::postUrl() const
{
    return m_postUrl;
}

void ThreadModel::fillTimeline(const QString &fromId, bool backwards)
{
    Q_UNUSED(fromId)
    Q_UNUSED(backwards)

    if (m_postId.isNull() || m_postId.isEmpty()) {
        return;
    }

    setLoading(true);

    if (!m_account) {
        m_account = AccountManager::instance().selectedAccount();
    }

    const auto statusUrl = m_account->apiUrl(QStringLiteral("/api/v1/statuses/%1").arg(m_postId));
    const auto contextUrl = m_account->apiUrl(QStringLiteral("/api/v1/statuses/%1/context").arg(m_postId));
    auto thread = std::make_shared<QList<Post *>>();

    auto handleError = [this](QNetworkReply *reply) {
        setLoading(false);
        Q_EMIT NetworkController::instance().networkErrorOccurred(reply->errorString());
    };

    auto onFetchContext = [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        const auto obj = doc.object();

        if (!doc.isObject()) {
            return;
        }

        auto ancestors = obj["ancestors"_L1].toArray().toVariantList();
        std::reverse(ancestors.begin(), ancestors.end());

        const auto descendents = obj["descendants"_L1].toArray();

        // If the root post has a non-zero reply count but no context from the server, it's possible that some replies are not available to us.
        if (descendents.isEmpty() && thread->front()->repliesCount() != 0) {
            m_hasHiddenReplies = true;
            Q_EMIT hasHiddenRepliesChanged();
        }

        m_rootPostIndex = ancestors.size();

        for (const auto &ancestor : ancestors) {
            if (ancestor.canConvert<QJsonObject>() || ancestor.canConvert<QVariantMap>()) {
                thread->push_front(new Post(m_account, ancestor.toJsonObject(), this));
            }
        }

        for (const auto &descendent : descendents) {
            if (!descendent.isObject()) {
                continue;
            }

            thread->push_back(new Post(m_account, descendent.toObject(), this));
        }

        beginResetModel();
        m_timeline = *thread;
        endResetModel();
        setLoading(false);

        Q_EMIT nameChanged(); // update title
    };

    auto onFetchStatus = [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        const auto obj = doc.object();

        if (!doc.isObject()) {
            return;
        }
        thread->push_front(new Post(m_account, obj, this));

        m_postUrl = thread->front()->url().toString();
        Q_EMIT postUrlChanged();

        m_account->get(contextUrl, true, this, onFetchContext, handleError);
    };

    m_account->get(statusUrl, true, this, onFetchStatus, handleError);
}

bool ThreadModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return false;
}

int ThreadModel::getRootIndex() const
{
    for (int i = 0; i < rowCount({}); i++) {
        if (data(index(i, 0), SelectedRole).toBool()) {
            return i;
        }
    }

    return -1;
}

void ThreadModel::reset()
{
    beginResetModel();
    qDeleteAll(m_timeline);
    m_timeline.clear();
    endResetModel();
}

void ThreadModel::refresh()
{
    reset();
    fillTimeline();
}

bool ThreadModel::hasHiddenReplies() const
{
    return m_hasHiddenReplies;
}

#include "moc_threadmodel.cpp"
