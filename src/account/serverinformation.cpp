// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "serverinformation.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>

#include "texthandler.h"

using namespace Qt::StringLiterals;

ServerInformation::ServerInformation(QObject *parent)
    : QObject(parent)
{
}

AbstractAccount *ServerInformation::account() const
{
    return m_account;
}

void ServerInformation::setAccount(AbstractAccount *account)
{
    if (m_account == account) {
        return;
    }
    m_account = account;
    Q_EMIT accountChanged();

    fetch();
}

QString ServerInformation::updatedAt() const
{
    return TextHandler::getRelativeDateTime(m_updatedAt);
}

QString ServerInformation::content() const
{
    return m_content;
}

QString ServerInformation::kind() const
{
    return m_kind;
}

void ServerInformation::setKind(const QString &kind)
{
    if (m_kind == kind) {
        return;
    }
    m_kind = kind;
    Q_EMIT kindChanged();

    fetch();
}

void ServerInformation::fetch()
{
    if (!m_account || m_kind.isEmpty()) {
        return;
    }
    m_account->get(m_account->apiUrl(QStringLiteral("/api/v1/instance/%1").arg(m_kind)), true, this, [this](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        m_updatedAt = QDateTime::fromString(doc["updated_at"_L1].toString(), Qt::ISODate).toLocalTime();
        m_content = doc["content"_L1].toString();

        Q_EMIT privacyPolicyChanged();
    });
}
