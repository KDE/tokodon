// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "preferences.h"
#include "identity.h"
#include "utils/utils.h"

#include <QHttpMultiPart>

Preferences::Preferences(AbstractAccount *account)
    : QObject(account)
    , m_account(account)
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

void Preferences::setDefaultVisibility(Post::Visibility visibility)
{
    setPreferencesField(QStringLiteral("source[privacy]"), visibilityToString(visibility));
}

bool Preferences::defaultSensitive() const
{
    return m_defaultSensitive;
}

void Preferences::setDefaultSensitive(bool sensitive)
{
    setPreferencesField(QStringLiteral("source[sensitive]"), sensitive ? QStringLiteral("true") : QStringLiteral("false"));
}

QString Preferences::defaultLanguage() const
{
    return m_defaultLanguage;
}

void Preferences::setDefaultLanguage(QString language)
{
    setPreferencesField(QStringLiteral("source[language]"), language);
}

QString Preferences::extendMedia() const
{
    return m_extendMedia;
}

bool Preferences::extendSpoiler() const
{
    return m_extendSpoiler;
}

void Preferences::setPreferencesField(QString name, QString value)
{
    auto multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart preferencesPart;
    preferencesPart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"").arg(name));
    preferencesPart.setBody(value.toUtf8());
    multiPart->append(preferencesPart);

    m_account->patch(m_account->apiUrl("/api/v1/accounts/update_credentials"), multiPart, true, this, [=](QNetworkReply *) {});
}