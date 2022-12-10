// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "accountmodel.h"
#include "abstractaccount.h"
#include "relationship.h"
#include <KLocalizedString>
#include <QCoreApplication>
#include <QUrlQuery>

AccountModel::AccountModel(AccountManager *manager, qint64 id, const QString &acct, QObject *parent)
    : TimelineModel(parent)
    , m_identity(nullptr)
    , m_id(id)
{
    setName(acct);
    setAccountManager(manager);

    connect(this, &AccountModel::identityChanged, this, &TimelineModel::nameChanged);

    if (!m_account->identityCached(acct)) {
        QUrl uriAccount(m_account->instanceUri());
        uriAccount.setPath(QString("/api/v1/accounts/%1").arg(id));

        manager->selectedAccount()->get(uriAccount, true, this, [this, manager, &acct](QNetworkReply *reply) {
            const auto data = reply->readAll();
            const auto doc = QJsonDocument::fromJson(data);

            m_identity = manager->selectedAccount()->identityLookup(acct, doc.object());
            Q_EMIT identityChanged();
            updateRelationships();
        });
    } else {
        const QJsonObject empty;
        m_identity = m_account->identityLookup(acct, empty);
    }

    if (m_account->identity().id() != m_identity->id()) {
        return;
    }

    // Fetch relationship. Don't cache this; it's lightweight.
    QUrl uriRelationship(m_account->instanceUri());
    uriRelationship.setPath(QStringLiteral("/api/v1/accounts/relationships"));
    uriRelationship.setQuery(QUrlQuery{{QStringLiteral("id[]"), QString::number(m_identity->id())}});

    m_account->get(uriRelationship, true, this, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (!doc.isArray()) {
            qDebug() << "Data returned from Relationship network request is not an array"
                     << "data: " << doc;
            return;
        }

        // We only are requesting for a single relationship, so doc should only contain one element
        m_identity->setRelationship(new Relationship(m_identity.get(), doc[0].toObject()));
        Q_EMIT identityChanged();
        updateRelationships();
    });
}

bool AccountModel::isSelf() const
{
    if(m_identity == nullptr)
        return false;

    return m_account->identity().id() == m_identity->id();
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
    m_fetching = true;

    auto thread = std::make_shared<QList<Post *>>();
    // Fetch pinned posts if we are starting from the top
    const auto fetchPinned = fromId.isNull();
    const auto excludeReplies = true;
    auto uriStatus = m_account->apiUrl(QString("/api/v1/accounts/%1/statuses").arg(m_id));

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

    auto uriPinned = m_account->apiUrl(QString("/api/v1/accounts/%1/statuses").arg(m_id));
    uriPinned.setQuery(QUrlQuery{{"pinned", "true"}});

    auto onFetchPinned = [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isArray()) {
            return;
        }

        int i = 0;
        const auto array = doc.array();
        for (const auto &value : array) {
            const QJsonObject obj = value.toObject();

            auto p = new Post(m_account, obj, this);
            thread->insert(i, p);
            i++;
        }

        fetchedTimeline(m_account, m_timelineName, *thread);
    };

    auto onFetchAccount = [=](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isArray()) {
            return;
        }

        const auto array = doc.array();
        for (const auto &value : array) {
            const QJsonObject obj = value.toObject();

            auto p = new Post(m_account, obj, this);
            thread->push_back(p);
        }

        if (fetchPinned) {
            m_account->get(uriPinned, true, this, onFetchPinned);
        } else {
            fetchedTimeline(m_account, m_timelineName, *thread);
        }
    };

    m_account->get(uriStatus, true, this, onFetchAccount);
}

Identity *AccountModel::identity() const
{
    return m_identity.get();
}

AbstractAccount *AccountModel::account() const
{
    return m_account;
}

void AccountModel::updateRelationships()
{
    if (m_account->identity().id() != m_identity->id()) {
        return;
    }

    // Fetch relationship. Don't cache this; it's lightweight.
    QUrl uriRelationship(m_account->instanceUri());
    uriRelationship.setPath(QStringLiteral("/api/v1/accounts/relationships"));
    uriRelationship.setQuery(QUrlQuery{{QStringLiteral("id[]"), QString::number(m_identity->id())}});

    m_account->get(uriRelationship, true, [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());
        if (!doc.isArray()) {
            qDebug() << "Data returned from Relationship network request is not an array"
                     << "data: " << doc;
            return;
        }

        // We only are requesting for a single relationship, so doc should only contain one element
        m_identity->setRelationship(new Relationship(m_identity.get(), doc[0].toObject()));
        Q_EMIT identityChanged();
    });
}