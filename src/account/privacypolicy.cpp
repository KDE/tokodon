// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "privacypolicy.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>

#include "texthandler.h"

using namespace Qt::StringLiterals;

PrivacyPolicy::PrivacyPolicy(QObject *parent)
    : QObject(parent)
{
}

AbstractAccount *PrivacyPolicy::account() const
{
    return m_account;
}

void PrivacyPolicy::setAccount(AbstractAccount *account)
{
    if (m_account == account) {
        return;
    }
    m_account = account;
    Q_EMIT accountChanged();

    if (!m_account) {
        return;
    }
    m_account->get(m_account->apiUrl(u"/api/v1/instance/privacy_policy"_s), true, this, [this](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        m_updatedAt = QDateTime::fromString(doc["updated_at"_L1].toString(), Qt::ISODate).toLocalTime();
        m_content = doc["content"_L1].toString();

        Q_EMIT privacyPolicyChanged();
    });
}

QString PrivacyPolicy::updatedAt() const
{
    return TextHandler::getRelativeDateTime(m_updatedAt);
}

QString PrivacyPolicy::content() const
{
    return m_content;
}
