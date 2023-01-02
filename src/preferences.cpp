// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "preferences.h"
#include "identity.h"
#include "utils.h"

Preferences::Preferences(AbstractAccount *account)
    : QObject(account)
{
    connect(account, &AbstractAccount::authenticated, this, [this, account]() {
        account->get(account->apiUrl(QStringLiteral("/api/v1/preferences")), true, this, [this](QNetworkReply *reply) {
            const auto obj = QJsonDocument::fromJson(reply->readAll()).object();
            m_defaultLanguage = obj[QStringLiteral("posting:default:language")].toString();
            m_defaultSensitive = obj[QStringLiteral("posting:default:sensitive")].toBool();
            m_defaultVisibility = stringToVisibility(obj[QStringLiteral("posting:default:visibility")].toString());
            m_extendSpoiler = obj[QStringLiteral("reading:expand:spoilers")].toBool();
            m_extendMedia = obj[QStringLiteral("reading:expand:media")].toString();
            Q_EMIT defaultVisibilityChanged();
            Q_EMIT defaultSensitiveChanged();
            Q_EMIT defaultLanguageChanged();
            Q_EMIT extendMediaChanged();
            Q_EMIT extendSpoilerChanged();
        });
    });
}

Post::Visibility Preferences::defaultVisibility() const
{
    return m_defaultVisibility;
}

bool Preferences::defaultSensitive() const
{
    return m_defaultSensitive;
}

QString Preferences::defaultLanguage() const
{
    return m_defaultLanguage;
}

QString Preferences::extendMedia() const
{
    return m_extendMedia;
}

bool Preferences::extendSpoiler() const
{
    return m_extendSpoiler;
}

