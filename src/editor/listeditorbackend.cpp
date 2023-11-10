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

bool ListEditorBackend::loading() const
{
    return m_loading;
}

void ListEditorBackend::create()
{
    m_loading = true;
    Q_EMIT loadingChanged();

    auto account = AccountManager::instance().selectedAccount();

    QUrlQuery formdata;

    formdata.addQueryItem(QStringLiteral("title"), m_title);
    formdata.addQueryItem(QStringLiteral("replies_policy"), m_repliesPolicy);
    formdata.addQueryItem(QStringLiteral("exclusive"), m_exclusive ? QStringLiteral("1") : QStringLiteral("0"));

    account->post(account->apiUrl(QStringLiteral("/api/v1/lists")), formdata, true, this, [=](QNetworkReply *) {
        Q_EMIT done();
    });
}

#include "moc_listeditorbackend.cpp"
