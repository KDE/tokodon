// SPDX-FileCopyrightText: 2026 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "editor/collectioneditorbackend.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>

#include "account/abstractaccount.h"
#include "account/accountmanager.h"

using namespace Qt::StringLiterals;

CollectionEditorBackend::CollectionEditorBackend(QObject *parent)
    : QObject(parent)
{
}

QString CollectionEditorBackend::collectionId() const
{
    return m_collectionId;
}

void CollectionEditorBackend::setCollectionId(const QString &collectionId)
{
    if (m_collectionId == collectionId) {
        return;
    }

    m_collectionId = collectionId;
    Q_EMIT collectionIdChanged();

    // load previous collection data
    m_loading = true;
    Q_EMIT loadingChanged();

    auto account = AccountManager::instance().selectedAccount();
    account->get(account->apiUrl(QStringLiteral("/api/v1/collections/%1").arg(m_collectionId)), true, this, [this](QNetworkReply *reply) {
        const auto document = QJsonDocument::fromJson(reply->readAll());

        const auto collectionObject = document["collection"_L1].toObject();

        m_name = collectionObject["name"_L1].toString();
        Q_EMIT nameChanged();

        m_description = collectionObject["description"_L1].toString();
        Q_EMIT descriptionChanged();

        m_sensitive = collectionObject["sensitive"_L1].toBool();
        Q_EMIT sensitiveChanged();

        m_discoverable = collectionObject["discoverable"_L1].toBool();
        Q_EMIT discoverableChanged();

        m_loading = false;
        Q_EMIT loadingChanged();
    });
}

bool CollectionEditorBackend::loading() const
{
    return m_loading;
}

void CollectionEditorBackend::submit()
{
    m_loading = true;
    Q_EMIT loadingChanged();

    auto account = AccountManager::instance().selectedAccount();

    QUrlQuery formdata;

    formdata.addQueryItem(QStringLiteral("name"), m_name);
    formdata.addQueryItem(QStringLiteral("description"), m_description);
    formdata.addQueryItem(QStringLiteral("sensitive"), m_sensitive ? QStringLiteral("true") : QStringLiteral("false"));
    formdata.addQueryItem(QStringLiteral("discoverable"), m_discoverable ? QStringLiteral("true") : QStringLiteral("false"));

    // If the collectionId is empty, then create a new collection
    if (m_collectionId.isEmpty()) {
        account->post(account->apiUrl(QStringLiteral("/api/v1/collections")), formdata, true, this, [this](QNetworkReply *) {
            Q_EMIT done();
        });
    } else {
        account->put(account->apiUrl(QStringLiteral("/api/v1/collections/%1").arg(m_collectionId)), formdata, true, this, [this](QNetworkReply *) {
            Q_EMIT done();
        });
    }
}

void CollectionEditorBackend::deleteCollection()
{
    Q_ASSERT(!m_collectionId.isEmpty());

    auto account = AccountManager::instance().selectedAccount();

    account->deleteResource(account->apiUrl(QStringLiteral("/api/v1/collections/%1").arg(m_collectionId)), true, this, [this, account](QNetworkReply *) {
        Q_EMIT done();
    });
}

#include "moc_collectioneditorbackend.cpp"
