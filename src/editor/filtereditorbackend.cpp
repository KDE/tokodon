// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "editor/filtereditorbackend.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>

#include "account/abstractaccount.h"
#include "account/accountmanager.h"

using namespace Qt::StringLiterals;

FilterEditorBackend::FilterEditorBackend(QObject *parent)
    : QObject(parent)
{
}

QString FilterEditorBackend::filterId() const
{
    return m_filterId;
}

void FilterEditorBackend::setFilterId(const QString &filterId)
{
    if (m_filterId == filterId) {
        return;
    }

    m_filterId = filterId;
    Q_EMIT filterIdChanged();

    // load previous list data
    m_loading = true;
    Q_EMIT loadingChanged();

    auto account = AccountManager::instance().selectedAccount();
    account->get(account->apiUrl(QStringLiteral("/api/v2/filters/%1").arg(m_filterId)), true, this, [this](QNetworkReply *reply) {
        const auto document = QJsonDocument::fromJson(reply->readAll());

        m_title = document["title"_L1].toString();
        Q_EMIT titleChanged();

        m_loading = false;
        Q_EMIT loadingChanged();
    });
}

bool FilterEditorBackend::loading() const
{
    return m_loading;
}

void FilterEditorBackend::submit()
{
    m_loading = true;
    Q_EMIT loadingChanged();

    auto account = AccountManager::instance().selectedAccount();

    QUrlQuery formdata;

    formdata.addQueryItem(QStringLiteral("title"), m_title);

    // If the filterId is empty, then create a new list
    if (m_filterId.isEmpty()) {
        account->post(account->apiUrl(QStringLiteral("/api/v2/filters")), formdata, true, this, [this](QNetworkReply *) {
            Q_EMIT done();
        });
    } else {
        account->put(account->apiUrl(QStringLiteral("/api/v2/filters/%1").arg(m_filterId)), formdata, true, this, [this](QNetworkReply *) {
            Q_EMIT done();
        });
    }
}

void FilterEditorBackend::deleteFilter()
{
    Q_ASSERT(!m_filterId.isEmpty());

    auto account = AccountManager::instance().selectedAccount();

    account->deleteResource(account->apiUrl(QStringLiteral("/api/v2/filters/%1").arg(m_filterId)), true, this, [this, account](QNetworkReply *) {
        account->removeFavoriteList(m_filterId);
        Q_EMIT done();
    });
}

#include "moc_filtereditorbackend.cpp"
