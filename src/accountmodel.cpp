// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "accountmodel.h"
#include "abstractaccount.h"
#include "relationship.h"
#include <KLocalizedString>
#include <QCoreApplication>
#include <QUrlQuery>

AccountModel::AccountModel(QObject *parent)
    : TimelineModel(parent)
{
    init();

    connect(this, &AccountModel::identityChanged, this, &TimelineModel::nameChanged);
}

bool AccountModel::isSelf() const
{
    if (m_identity == nullptr) {
        return false;
    }

    return m_account->identity()->id() == m_identity->id();
}

QString AccountModel::displayName() const
{
    if (!m_identity) {
        return i18n("Loading");
    }

    return m_identity->displayNameHtml();
}

void AccountModel::fillTimeline(const QString &fromId)
{
    if (m_accountId.isEmpty() || m_accountId.isNull()) {
        return;
    }
    setLoading(true);

    // Fetch pinned posts if we are starting from the top
    const auto fetchPinned = fromId.isNull();
    const auto excludeReplies = true;
    auto uriStatus = m_account->apiUrl(QStringLiteral("/api/v1/accounts/%1/statuses").arg(m_accountId));

    auto statusQuery = QUrlQuery();
    if (excludeReplies) {
        statusQuery.addQueryItem("exclude_replies", "true");
    }
    if (!fetchPinned) {
        statusQuery.addQueryItem("max_id", fromId);
    }
    if (!statusQuery.isEmpty()) {
        uriStatus.setQuery(statusQuery);
    }

    auto uriPinned = m_account->apiUrl(QStringLiteral("/api/v1/accounts/%1/statuses").arg(m_accountId));
    uriPinned.setQuery(QUrlQuery{{
        "pinned",
        "true",
    }});

    const auto account = m_account;
    const auto id = m_accountId;

    auto handleError = [this](QNetworkReply *reply) {
        Q_UNUSED(reply);
        setLoading(false);
    };

    auto onFetchPinned = [this, id, account](QNetworkReply *reply) {
        if (m_account != account || m_accountId != id) {
            setLoading(false);
            return;
        }
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        if (!doc.isArray()) {
            setLoading(false);
            return;
        }
        const auto array = doc.array();
        if (array.isEmpty()) {
            setLoading(false);
            return;
        }

        QList<Post *> posts;
        std::transform(array.cbegin(), array.cend(), std::back_inserter(posts), [this](const QJsonValue &value) {
            auto post = new Post(m_account, value.toObject(), this);
            post->m_pinned = true;
            return post;
        });
        beginInsertRows({}, 0, posts.size() - 1);
        m_timeline = posts + m_timeline;
        endInsertRows();
        setLoading(false);
    };

    auto onFetchAccount = [account, id, fetchPinned, uriPinned, handleError, onFetchPinned, this](QNetworkReply *reply) {
        if (m_account != account || m_accountId != id) {
            setLoading(false);
            return;
        }

        fetchedTimeline(reply->readAll());
        if (fetchPinned) {
            m_account->get(uriPinned, true, this, onFetchPinned, handleError);
        } else {
            setLoading(false);
        }
    };

    m_account->get(uriStatus, true, this, onFetchAccount, handleError);
}

Identity *AccountModel::identity() const
{
    return m_identity.get();
}

QString AccountModel::accountId() const
{
    return m_accountId;
}

void AccountModel::setAccountId(const QString &accountId)
{
    if (accountId == m_accountId || accountId.isEmpty()) {
        return;
    }
    m_accountId = accountId;
    Q_EMIT accountIdChanged();

    if (!m_account->identityCached(accountId)) {
        QUrl uriAccount(m_account->instanceUri());
        uriAccount.setPath(QStringLiteral("/api/v1/accounts/%1").arg(accountId));

        m_account->get(uriAccount, true, this, [this, accountId](QNetworkReply *reply) {
            const auto data = reply->readAll();
            const auto doc = QJsonDocument::fromJson(data);

            m_identity = m_account->identityLookup(accountId, doc.object());
            Q_EMIT identityChanged();
            updateRelationships();
        });
    } else {
        m_identity = m_account->identityLookup(accountId, {});
        Q_EMIT identityChanged();
        updateRelationships();
    }
    Q_EMIT accountIdChanged();

    fillTimeline();
}

AbstractAccount *AccountModel::account() const
{
    return m_account;
}

void AccountModel::updateRelationships()
{
    if (m_account->identity()->id() == m_identity->id()) {
        return;
    }

    // Fetch relationship. Don't cache this; it's lightweight.
    QUrl uriRelationship(m_account->instanceUri());
    uriRelationship.setPath(QStringLiteral("/api/v1/accounts/relationships"));
    uriRelationship.setQuery(QUrlQuery{
        {QStringLiteral("id[]"), m_identity->id()},
    });

    m_account->get(uriRelationship, true, this, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (!doc.isArray()) {
            qWarning() << "Data returned from Relationship network request is not an array"
                       << "data: " << doc;
            return;
        }

        // We only are requesting for a single relationship, so doc should only contain one element
        m_identity->setRelationship(new Relationship(m_identity.get(), doc[0].toObject()));
        Q_EMIT identityChanged();
    });
}