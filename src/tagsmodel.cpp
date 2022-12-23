// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "tagsmodel.h"
#include "abstractaccount.h"
#include <QUrlQuery>

TagsModel::TagsModel(QObject *parent)
    : TimelineModel(QStringLiteral("None"), parent)
{
    init();
}

TagsModel::~TagsModel() = default;

QString TagsModel::hashtag() const
{
    return m_hashtag;
}

void TagsModel::setHashtag(const QString &hashtag)
{
    if (hashtag == m_hashtag) {
        return;
    }
    m_hashtag = hashtag;
    Q_EMIT hashtagChanged();
    fillTimeline({});
}

QString TagsModel::displayName() const
{
    return QLatin1Char('#') + m_hashtag;
}

void TagsModel::fillTimeline(const QString &fromId)
{
    if (m_hashtag.isEmpty()) {
        return;
    }
    QUrlQuery q;
    if (!fromId.isEmpty()) {
        q.addQueryItem("max_id", fromId);
    }
    auto uri = m_account->apiUrl(QString("/api/v1/timelines/tag/%1").arg(m_hashtag));
    uri.setQuery(q);
    const auto account = m_account;
    const auto hashtag = m_hashtag;
    m_account->get(uri, true, this, [this, account, hashtag, uri](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isArray()) {
            return;
        }

        if (account != m_account || m_hashtag != hashtag) {
            // Receiving request for an old query
            m_loading = false;
            Q_EMIT loadingChanged();
            return;
        }

        QList<Post *> posts;

        const auto array = doc.array();
        for (const auto &value : array) {
            posts.push_back(new Post(m_account, value.toObject(), this));
        }

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
    });
}
