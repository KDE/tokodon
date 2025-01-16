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

        const QJsonArray context = document["context"_L1].toArray();

        m_homeAndListsContext = context.contains("home"_L1);
        Q_EMIT homeAndListsContextChanged();

        m_notificationsContext = context.contains("notifications"_L1);
        Q_EMIT notificationsContextChanged();

        m_publicTimelinesContext = context.contains("public"_L1);
        Q_EMIT publicTimelinesContextChanged();

        m_conversationsContext = context.contains("thread"_L1);
        Q_EMIT conversationsContextChanged();

        m_profilesContext = context.contains("account"_L1);
        Q_EMIT profilesContextChanged();

        m_hideCompletely = document["filter_action"_L1] == "hide"_L1;
        Q_EMIT hideCompletelyChanged();

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

    if (m_homeAndListsContext) {
        formdata.addQueryItem(QStringLiteral("context[]"), QStringLiteral("home"));
    }
    if (m_notificationsContext) {
        formdata.addQueryItem(QStringLiteral("context[]"), QStringLiteral("notifications"));
    }
    if (m_publicTimelinesContext) {
        formdata.addQueryItem(QStringLiteral("context[]"), QStringLiteral("public"));
    }
    if (m_conversationsContext) {
        formdata.addQueryItem(QStringLiteral("context[]"), QStringLiteral("thread"));
    }
    if (m_profilesContext) {
        formdata.addQueryItem(QStringLiteral("context[]"), QStringLiteral("account"));
    }

    if (m_hideCompletely) {
        formdata.addQueryItem(QStringLiteral("filter_action"), QStringLiteral("hide"));
    } else {
        formdata.addQueryItem(QStringLiteral("filter_action"), QStringLiteral("warn"));
    }

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
