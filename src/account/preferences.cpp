// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "account/preferences.h"

#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "account/abstractaccount.h"

using namespace Qt::StringLiterals;

Preferences::Preferences(AbstractAccount *account)
    : QObject(account)
    , m_account(account)
{
    connect(account, &AbstractAccount::authenticated, this, [this, account]() {
        account->get(account->apiUrl(QStringLiteral("/api/v1/preferences")), true, this, [this](QNetworkReply *reply) {
            const auto obj = QJsonDocument::fromJson(reply->readAll()).object();

            if (const auto defaultLanguage = obj["posting:default:language"_L1]; !defaultLanguage.isNull()) {
                m_defaultLanguage = defaultLanguage.toString();
            }

            m_defaultSensitive = obj["posting:default:sensitive"_L1].toBool();
            m_defaultVisibility = Post::stringToVisibility(obj["posting:default:visibility"_L1].toString());
            m_extendSpoiler = obj["reading:expand:spoilers"_L1].toBool();
            m_extendMedia = obj["reading:expand:media"_L1].toString();
            m_indexable = obj["indexable"_L1].toBool();
            m_hideCollections = obj["hide_collections"_L1].toBool();
            Q_EMIT defaultVisibilityChanged();
            Q_EMIT defaultSensitiveChanged();
            Q_EMIT defaultLanguageChanged();
            Q_EMIT extendMediaChanged();
            Q_EMIT extendSpoilerChanged();
            Q_EMIT indexableChanged();
            Q_EMIT hideCollectionsChanged();
        });
    });
}

Post::Visibility Preferences::defaultVisibility() const
{
    return m_defaultVisibility;
}

void Preferences::setDefaultVisibility(const Post::Visibility visibility)
{
    if (visibility == m_defaultVisibility) {
        return;
    }

    m_defaultVisibility = visibility;
    Q_EMIT defaultVisibilityChanged();

    setPreferencesField(QStringLiteral("source[privacy]"), Post::visibilityToString(visibility));
}

bool Preferences::defaultSensitive() const
{
    return m_defaultSensitive;
}

void Preferences::setDefaultSensitive(const bool sensitive)
{
    if (sensitive == m_defaultSensitive) {
        return;
    }

    m_defaultSensitive = sensitive;
    Q_EMIT defaultSensitiveChanged();

    setPreferencesField(QStringLiteral("source[sensitive]"), sensitive ? QStringLiteral("true") : QStringLiteral("false"));
}

QString Preferences::defaultLanguage() const
{
    if (!m_defaultLanguage.isEmpty()) {
        return m_defaultLanguage;
    } else {
        return QStringLiteral("en");
    }
}

void Preferences::setDefaultLanguage(const QString &language)
{
    if (language == m_defaultLanguage) {
        return;
    }

    m_defaultLanguage = language;
    Q_EMIT defaultLanguageChanged();

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

bool Preferences::indexable() const
{
    return m_indexable;
}

void Preferences::setIndexable(bool indexable)
{
    if (indexable == m_indexable) {
        return;
    }

    m_indexable = indexable;
    Q_EMIT indexableChanged();

    setPreferencesField(QStringLiteral("indexable"), indexable ? QStringLiteral("true") : QStringLiteral("false"));
}

bool Preferences::hideCollections() const
{
    return m_hideCollections;
}

void Preferences::setHideCollections(bool hide)
{
    if (hide == m_hideCollections) {
        return;
    }

    m_hideCollections = hide;
    Q_EMIT hideCollectionsChanged();

    setPreferencesField(QStringLiteral("hide_collections"), hide ? QStringLiteral("true") : QStringLiteral("false"));
}

void Preferences::setPreferencesField(const QString &name, const QString &value)
{
    const auto multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart preferencesPart;
    preferencesPart.setHeader(QNetworkRequest::ContentDispositionHeader, QStringLiteral("form-data; name=\"%1\"").arg(name));
    preferencesPart.setBody(value.toUtf8());
    multiPart->append(preferencesPart);

    m_account->patch(m_account->apiUrl(QStringLiteral("/api/v1/accounts/update_credentials")), multiPart, true, this, [](QNetworkReply *) {});
}

#include "moc_preferences.cpp"
