// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "reporteditorbackend.h"

#include <QUrlQuery>

#include "abstractaccount.h"
#include "accountmanager.h"

using namespace Qt::StringLiterals;

ReportEditorBackend::ReportEditorBackend(QObject *parent)
    : QObject(parent)
{
}

bool ReportEditorBackend::loading() const
{
    return m_loading;
}

void ReportEditorBackend::submit()
{
    m_loading = true;
    Q_EMIT loadingChanged();

    auto account = AccountManager::instance().selectedAccount();

    QUrlQuery formdata;

    formdata.addQueryItem(QStringLiteral("account_id"), m_accountId);
    if (!m_comment.isEmpty()) {
        formdata.addQueryItem(QStringLiteral("comment"), m_comment);
    }
    if (!m_postId.isEmpty()) {
        formdata.addQueryItem(QStringLiteral("status_ids[]"), m_postId);
    }

    account->post(account->apiUrl(QStringLiteral("/api/v1/reports")), formdata, true, this, [=](QNetworkReply *reply) {
        Q_EMIT reported();
    });
}

#include "moc_reporteditorbackend.cpp"
