// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "timeline/accountmediatimelinemodel.h"

#include <KLocalizedString>

using namespace Qt::StringLiterals;

AccountMediaTimelineModel::AccountMediaTimelineModel(QObject *parent)
    : AbstractListModel(parent)
{
    connect(this, &AccountMediaTimelineModel::filtersChanged, this, [this] {
        reset();
        fillTimeline();
    });
}

QString AccountMediaTimelineModel::displayName() const
{
    return {};
}

void AccountMediaTimelineModel::fillTimeline(const QString &fromId)
{
    if (m_accountId.isEmpty() || m_accountId.isNull()) {
        return;
    }
    setLoading(true);

    // Fetch pinned posts if we are starting from the top
    auto uriStatus = account()->apiUrl(QStringLiteral("/api/v1/accounts/%1/statuses").arg(m_accountId));

    auto statusQuery = QUrlQuery();
    statusQuery.addQueryItem(QStringLiteral("only_media"), QStringLiteral("true"));
    if (!m_tagged.isEmpty()) {
        statusQuery.addQueryItem(QStringLiteral("tagged"), m_tagged);
    }
    if (!fromId.isNull()) {
        statusQuery.addQueryItem(QStringLiteral("max_id"), fromId);
    }
    if (!statusQuery.isEmpty()) {
        uriStatus.setQuery(statusQuery);
    }

    auto uriPinned = account()->apiUrl(QStringLiteral("/api/v1/accounts/%1/statuses").arg(m_accountId));
    uriPinned.setQuery(QUrlQuery{{
        QStringLiteral("pinned"),
        QStringLiteral("true"),
    }});

    const auto id = m_accountId;

    auto handleError = [this](QNetworkReply *reply) {
        Q_UNUSED(reply);
        setLoading(false);
    };

    auto onFetchAccount = [id, fromId, this](QNetworkReply *reply) {
        if (m_accountId != id) {
            setLoading(false);
            return;
        }

        // if we just restarted the fetch (fromId is null) then we must clear the previous array
        // this can happen if we just entered the profile page (okay, just a no-op) or if the filters change
        if (fromId.isNull()) {
            reset();
        }

        QList<MediaAttachment> attachments;
        const auto doc = QJsonDocument::fromJson(reply->readAll());

        if (!doc.isArray()) {
            setLoading(false);
            return;
        }

        const auto array = doc.array();

        if (array.isEmpty()) {
            setLoading(false);
            return;
        }

        for (const auto &item : array) {
            for (const auto &attachmentObj : item.toObject()["media_attachments"_L1].toArray()) {
                MediaAttachment attachment;
                attachment.attachment = new Attachment(attachmentObj.toObject(), this);
                attachment.postId = item["id"_L1].toString();
                attachment.sensitive = item["sensitive"_L1].toBool();

                attachments.push_back(attachment);
            }
        }

        if (!m_timeline.isEmpty()) {
            beginInsertRows({}, m_timeline.size(), m_timeline.size() + attachments.size() - 1);
            m_timeline += attachments;
            endInsertRows();
        } else {
            beginInsertRows({}, 0, attachments.size() - 1);
            m_timeline = attachments;
            endInsertRows();
        }

        setLoading(false);
    };

    account()->get(uriStatus, true, this, onFetchAccount, handleError);
}

QVariant AccountMediaTimelineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    auto post = m_timeline[index.row()];
    switch (role) {
    case PostIdRole:
        return post.postId;
    case SourceRole:
        return post.attachment->m_preview_url;
    case TempSourceRole:
        return post.attachment->tempSource();
    case FocusXRole:
        return post.attachment->focusX();
    case FocusYRole:
        return post.attachment->focusY();
    case SensitiveRole:
        return post.sensitive;
    case AttachmentRole:
        return QVariant::fromValue(post.attachment);
    }
    return {};
}

int AccountMediaTimelineModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_timeline.size();
}

QHash<int, QByteArray> AccountMediaTimelineModel::roleNames() const
{
    return {
        {PostIdRole, QByteArrayLiteral("postId")},
        {SourceRole, QByteArrayLiteral("source")},
        {TempSourceRole, QByteArrayLiteral("tempSource")},
        {FocusXRole, QByteArrayLiteral("focusX")},
        {FocusYRole, QByteArrayLiteral("focusY")},
        {SensitiveRole, QByteArrayLiteral("sensitive")},
        {AttachmentRole, QByteArrayLiteral("attachment")},
    };
}

QString AccountMediaTimelineModel::accountId() const
{
    return m_accountId;
}

void AccountMediaTimelineModel::setAccountId(const QString &accountId)
{
    if (accountId == m_accountId || accountId.isEmpty()) {
        return;
    }
    m_accountId = accountId;
    Q_EMIT accountIdChanged();

    fillTimeline();
}

void AccountMediaTimelineModel::reset()
{
    beginResetModel();
    for (const auto &item : m_timeline) {
        item.attachment->deleteLater();
    }
    m_timeline.clear();
    endResetModel();
}

bool AccountMediaTimelineModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return true;
}

void AccountMediaTimelineModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (m_timeline.empty()) {
        return;
    }

    const auto p = m_timeline.last();
    fillTimeline(p.postId);
}

#include "moc_accountmediatimelinemodel.cpp"