// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "threadmodel.h"
#include "abstractaccount.h"
#include <KLocalizedString>
#include <QJsonObject>

ThreadModel::ThreadModel(const QString &postId, QObject *parent)
    : TimelineModel(parent)
    , m_postId(postId)
{
    setAccountManager(&AccountManager::instance());
}

QString ThreadModel::displayName() const
{
    return i18nc("@title", "Thread");
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

void ThreadModel::fillTimeline(const QString &from_id)
{
    Q_UNUSED(from_id)

    m_fetching = true;

    auto statusUrl = m_account->apiUrl(QString("/api/v1/statuses/%1").arg(m_postId));
    auto contextUrl = m_account->apiUrl(QString("/api/v1/statuses/%1/context").arg(m_postId));
    auto thread = std::make_shared<QList<Post *>>();

    auto onFetchContext = [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        const auto obj = doc.object();

        if (!doc.isObject()) {
            return;
        }

        const auto ancestors = obj["ancestors"].toArray();

        for (const auto &anc : ancestors) {
            if (!anc.isObject()) {
                continue;
            }

            const auto anc_obj = anc.toObject();
            const auto p = new Post(m_account, anc_obj, this);

            thread->push_front(p);
        }

        const auto descendents = obj["descendants"].toArray();

        for (const auto &desc : descendents) {
            if (!desc.isObject()) {
                continue;
            }

            const auto desc_obj = desc.toObject();
            const auto p = new Post(m_account, desc_obj, this);

            thread->push_back(p);
        }

        beginResetModel();
        m_timeline = *thread;
        endResetModel();
    };

    auto onFetchStatus = [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        const auto obj = doc.object();

        if (!doc.isObject()) {
            return;
        }

        const auto p = new Post(m_account, obj, this);
        thread->push_front(p);

        m_account->get(contextUrl, true, this, onFetchContext);
    };

    m_account->get(statusUrl, true, this, onFetchStatus);
}

bool ThreadModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return false;
}
