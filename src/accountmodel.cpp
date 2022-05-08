// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "accountmodel.h"
#include <KLocalizedString>
#include <QCoreApplication>

AccountModel::AccountModel(AccountManager *manager, qint64 id, const QString &acct, QObject *parent)
    : TimelineModel(parent)
    , m_identity(nullptr)
    , m_id(id)
{
    setName(acct);
    setAccountManager(manager);

    if (!m_account->identityCached(acct)) {
        QUrl uriAccount(m_account->instanceUri());
        uriAccount.setPath(QString("/api/v1/accounts/%1").arg(id));

        manager->selectedAccount()->get(uriAccount, true, [this, manager, &acct](QNetworkReply *reply) {
            const auto data = reply->readAll();
            const auto doc = QJsonDocument::fromJson(data);

            if (!doc.isArray()) {
                qDebug() << data;
                return;
            }

            m_identity = manager->selectedAccount()->identityLookup(acct, doc.object());
            Q_EMIT identityChanged();
        });
    } else {
        const QJsonObject empty;
        m_identity = m_account->identityLookup(acct, empty);
        Q_EMIT identityChanged();
    }

    if (m_account->identity().id() != m_identity->id()) {
        return;
    }

    // Fetch relationship. Don't cache this; it's lightweight.
    QUrl uriRelationship(m_account->instanceUri());
    uriRelationship.setPath(QStringLiteral("/api/v1/accounts/relationships"));
    uriRelationship.setQuery(QUrlQuery{{QStringLiteral("id[]"), QString::number(m_identity->m_id)}});

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

bool AccountModel::isSelf() const
{
    return m_account->identity().id() == m_identity->id();
}

QString AccountModel::displayName() const
{
    if (!m_identity) {
        return i18n("Loading");
    }

    return m_identity->m_display_name;
}

void AccountModel::fillTimeline(const QString &fromId)
{
    m_fetching = true;

    m_account->fetchAccount(m_id, true, m_timelineName, fromId);
}

Identity *AccountModel::identity() const
{
    return m_identity.get();
}

Account *AccountModel::account() const
{
    return m_account;
}
