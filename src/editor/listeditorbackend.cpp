// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "editor/listeditorbackend.h"

#include <KLocalizedString>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>

#include "account/abstractaccount.h"
#include "account/accountmanager.h"

using namespace Qt::StringLiterals;

const QStringList supportedPolicies{QStringLiteral("none"), QStringLiteral("list"), QStringLiteral("followed")};

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
    m_favorite = account->isFavoriteList(m_listId);
    Q_EMIT favoriteChanged();

    account->get(account->apiUrl(QStringLiteral("/api/v1/lists/%1").arg(m_listId)), true, this, [this](QNetworkReply *reply) {
        const auto document = QJsonDocument::fromJson(reply->readAll());

        m_title = document["title"_L1].toString();
        Q_EMIT titleChanged();

        m_repliesPolicy = document["replies_policy"_L1].toString();

        m_exclusive = document["exclusive"_L1].toBool();
        Q_EMIT exclusiveChanged();

        m_loading = false;
        Q_EMIT loadingChanged();
    });
}

int ListEditorBackend::replyPolicyIndex() const
{
    return supportedPolicies.indexOf(m_repliesPolicy);
}

void ListEditorBackend::setReplyPolicyIndex(int index)
{
    m_repliesPolicy = supportedPolicies[index];
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
        account->post(account->apiUrl(QStringLiteral("/api/v1/lists")), formdata, true, this, [this](QNetworkReply *) {
            Q_EMIT done();
        });
    } else {
        if (m_favorite) {
            account->addFavoriteList(m_listId);
        } else {
            account->removeFavoriteList(m_listId);
        }

        account->put(account->apiUrl(QStringLiteral("/api/v1/lists/%1").arg(m_listId)), formdata, true, this, [this](QNetworkReply *) {
            Q_EMIT done();
        });
    }
}

void ListEditorBackend::deleteList()
{
    Q_ASSERT(!m_listId.isEmpty());

    auto account = AccountManager::instance().selectedAccount();

    account->deleteResource(account->apiUrl(QStringLiteral("/api/v1/lists/%1").arg(m_listId)), true, this, [this, account](QNetworkReply *) {
        account->removeFavoriteList(m_listId);
        Q_EMIT done();
    });
}

QStringList ListEditorBackend::replyPolicies()
{
    return {i18n("No one"), i18n("Members of the list"), i18n("Any followed users")};
}

#include "moc_listeditorbackend.cpp"
