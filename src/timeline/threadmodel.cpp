// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "threadmodel.h"
#include "account/abstractaccount.h"
#include "timelinemodel.h"
#include <KLocalizedString>
#include <QJsonObject>
#include <qstringliteral.h>

ThreadModel::ThreadModel(QObject *parent)
    : TimelineModel(parent)
{
    init();
}

QVariant ThreadModel::data(const QModelIndex &index, int role) const
{
    if (role == SelectedRole) {
        return m_postId == TimelineModel::data(index, IdRole).toString();
    }

    return TimelineModel::data(index, role);
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

void ThreadModel::fillTimeline(const QString &fromId)
{
    Q_UNUSED(fromId)

    if (m_postId.isNull() || m_postId.isEmpty()) {
        return;
    }

    setLoading(true);

    if (!m_account) {
        m_account = AccountManager::instance().selectedAccount();
    }

    const auto statusUrl = m_account->apiUrl(QString("/api/v1/statuses/%1").arg(m_postId));
    const auto contextUrl = m_account->apiUrl(QString("/api/v1/statuses/%1/context").arg(m_postId));
    auto thread = std::make_shared<QList<Post *>>();

    auto handleError = [this](QNetworkReply *reply) {
        Q_UNUSED(reply);
        setLoading(false);
    };

    auto onFetchContext = [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        const auto obj = doc.object();

        if (!doc.isObject()) {
            return;
        }

        auto ancestors = obj["ancestors"].toArray().toVariantList();
        std::reverse(ancestors.begin(), ancestors.end());

        for (const auto &ancestor : ancestors) {
            if (ancestor.canConvert<QJsonObject>() || ancestor.canConvert<QVariantMap>()) {
                thread->push_front(new Post(m_account, ancestor.toJsonObject(), this));
            }
        }

        const auto descendents = obj["descendants"].toArray();

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
    };

    auto onFetchStatus = [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        const auto obj = doc.object();

        if (!doc.isObject()) {
            return;
        }
        thread->push_front(new Post(m_account, obj, this));
        m_account->get(contextUrl, true, this, onFetchContext, handleError);
    };

    m_account->get(statusUrl, true, this, onFetchStatus, handleError);
}

bool ThreadModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return false;
}
