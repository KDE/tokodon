// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "datatypes/card.h"

#include "account/abstractaccount.h"

using namespace Qt::StringLiterals;

Card::Card(AbstractAccount *account, QJsonObject card)
    : m_card(card)
    , m_account(account)
{
}

QString Card::authorName() const
{
    if (const auto author = authorObject()) {
        return (*author)["name"_L1].toString();
    }
    return m_card[QLatin1String("author_name")].toString();
}

QString Card::authorUrl() const
{
    if (const auto author = authorObject()) {
        return (*author)["url"_L1].toString();
    }
    return m_card[QLatin1String("author_url")].toString();
}

Identity *Card::authorIdentity() const
{
    if (m_account != nullptr) {
        if (const auto author = authorObject()) {
            const auto account = (*author)["account"_L1];
            if (!account.isNull()) {
                return m_account->identityLookup(account["id"_L1].toString(), account.toObject()).get();
            }
        }
    }
    return nullptr;
}

QString Card::blurhash() const
{
    return m_card[QLatin1String("blurhash")].toString();
}

QString Card::description() const
{
    return m_card[QLatin1String("description")].toString();
}

QString Card::embedUrl() const
{
    return m_card[QLatin1String("embed_url")].toString();
}

int Card::width() const
{
    return m_card[QLatin1String("weight")].toInt();
}

int Card::height() const
{
    return m_card[QLatin1String("height")].toInt();
}

QString Card::html() const
{
    return m_card[QLatin1String("html")].toString();
}

QString Card::image() const
{
    return m_card[QLatin1String("image")].toString();
}

QString Card::providerName() const
{
    const auto providerName = m_card[QLatin1String("provider_name")].toString();
    if (!providerName.isEmpty()) {
        return providerName;
    }
    return url().host();
}

QString Card::providerUrl() const
{
    return m_card[QLatin1String("provider_url")].toString();
}

QString Card::title() const
{
    return m_card[QLatin1String("title")].toString().trimmed();
}

QUrl Card::url() const
{
    return QUrl::fromUserInput(m_card[QLatin1String("url")].toString());
}

std::optional<QJsonObject> Card::authorObject() const
{
    if (m_card.contains("authors"_L1)) {
        const auto &authors = m_card["authors"_L1].toArray();
        if (!authors.isEmpty()) {
            return authors.first().toObject();
        }
    }
    return std::nullopt;
}
