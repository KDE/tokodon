// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "listeditorbackend.h"

#include "abstractaccount.h"
#include "accountmanager.h"

using namespace Qt::StringLiterals;

ListEditorBackend::ListEditorBackend(QObject *parent)
    : QObject(parent)
{
}

QString ListEditorBackend::listId() const
{
    return m_listId;
}

void ListEditorBackend::setListId(const QString &listId)
{
    if (m_listId == listId) {
        return;
    }

    m_listId = listId;
    Q_EMIT listIdChanged();

    // load previous list data
    m_loading = true;
    Q_EMIT loadingChanged();

    auto account = AccountManager::instance().selectedAccount();

    account->get(account->apiUrl(QStringLiteral("/api/v1/lists/%1").arg(m_listId)), true, this, [=](QNetworkReply *reply) {
        const auto document = QJsonDocument::fromJson(reply->readAll());

        m_title = document["title"_L1].toString();
        Q_EMIT titleChanged();

        m_exclusive = document["exclusive"_L1].toBool();
        Q_EMIT exclusiveChanged();

        m_loading = false;
        Q_EMIT loadingChanged();
    });
}

bool ListEditorBackend::loading() const
{
    return m_loading;
}

void ListEditorBackend::submit()
{
    m_loading = true;
    Q_EMIT loadingChanged();

    auto account = AccountManager::instance().selectedAccount();

    QUrlQuery formdata;

    formdata.addQueryItem(QStringLiteral("title"), m_title);
    formdata.addQueryItem(QStringLiteral("replies_policy"), m_repliesPolicy);
    formdata.addQueryItem(QStringLiteral("exclusive"), m_exclusive ? QStringLiteral("1") : QStringLiteral("0"));

    // If the listId is empty, then create a new list
    if (m_listId.isEmpty()) {
        account->post(account->apiUrl(QStringLiteral("/api/v1/lists")), formdata, true, this, [=](QNetworkReply *) {
            Q_EMIT done();
        });
    } else {
        account->put(account->apiUrl(QStringLiteral("/api/v1/lists/%1").arg(m_listId)), formdata, true, this, [=](QNetworkReply *) {
            Q_EMIT done();
        });
    }
}

#include "moc_listeditorbackend.cpp"
