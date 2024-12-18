// SPDX-FileCopyrightText: 2022 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "timeline/tagstimelinemodel.h"

#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>

#include "networkcontroller.h"

using namespace Qt::StringLiterals;

TagsTimelineModel::TagsTimelineModel(QObject *parent)
    : TimelineModel(parent)
{
    init();
}

TagsTimelineModel::~TagsTimelineModel() = default;

QString TagsTimelineModel::hashtag() const
{
    return m_hashtag;
}

void TagsTimelineModel::setHashtag(const QString &hashtag)
{
    if (hashtag == m_hashtag) {
        return;
    }
    m_hashtag = hashtag;
    Q_EMIT hashtagChanged();
    fillTimeline({});
}

void TagsTimelineModel::follow()
{
    m_account->post(m_account->apiUrl(QStringLiteral("/api/v1/tags/%1/follow").arg(m_hashtag)), QJsonDocument{}, true, this, [this](QNetworkReply *reply) {
        Q_UNUSED(reply);
        m_following = true;
        Q_EMIT followedChanged();
    });
}

void TagsTimelineModel::unfollow()
{
    m_account->post(m_account->apiUrl(QStringLiteral("/api/v1/tags/%1/unfollow").arg(m_hashtag)), QJsonDocument{}, true, this, [this](QNetworkReply *reply) {
        Q_UNUSED(reply);
        m_following = false;
        Q_EMIT followedChanged();
    });
}

QString TagsTimelineModel::displayName() const
{
    return QLatin1Char('#') + m_hashtag;
}

void TagsTimelineModel::fillTimeline(const QString &fromId, bool backwards)
{
    Q_UNUSED(backwards)
    if (m_hashtag.isEmpty()) {
        return;
    }
    QUrlQuery q;
    if (!fromId.isEmpty()) {
        q.addQueryItem(QStringLiteral("max_id"), fromId);
    }
    auto uri = m_account->apiUrl(QStringLiteral("/api/v1/timelines/tag/%1").arg(m_hashtag));
    uri.setQuery(q);
    const auto account = m_account;
    const auto hashtag = m_hashtag;

    auto handleError = [this](QNetworkReply *reply) {
        setLoading(false);
        NetworkController::instance().networkErrorOccurred(reply->errorString());
    };

    setLoading(true);
    m_account->get(
        uri,
        true,
        this,
        [this, account, hashtag, uri](QNetworkReply *reply) {
            if (account != m_account || m_hashtag != hashtag) {
                // Receiving request for an old query
                setLoading(false);
                return;
            }

            const QByteArray data = reply->readAll();
            const QJsonDocument doc = QJsonDocument::fromJson(data);
            m_following = doc["following"_L1].toBool();

            fetchedTimeline(data);
            setLoading(false);
        },
        handleError);
}

void TagsTimelineModel::reset()
{
    beginResetModel();
    qDeleteAll(m_timeline);
    m_timeline.clear();
    endResetModel();
}

bool TagsTimelineModel::following() const
{
    return m_following;
}

#include "moc_tagstimelinemodel.cpp"
