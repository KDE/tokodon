// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QJsonObject>

#include "account/identity.h"

class AbstractAccount;

class Card
{
    Q_GADGET

    Q_PROPERTY(QString authorName READ authorName)
    Q_PROPERTY(QString authorUrl READ authorUrl)
    Q_PROPERTY(Identity *authorIdentity READ authorIdentity)
    Q_PROPERTY(QString blurhash READ blurhash)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QString embedUrl READ embedUrl)
    Q_PROPERTY(int width READ width)
    Q_PROPERTY(int height READ height)
    Q_PROPERTY(QString html READ html)
    Q_PROPERTY(QString image READ image)
    Q_PROPERTY(QString providerName READ providerName)
    Q_PROPERTY(QString providerUrl READ providerUrl)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QUrl url READ url)

public:
    Card() = default;
    explicit Card(AbstractAccount *account, QJsonObject card);

    [[nodiscard]] QString authorName() const;
    [[nodiscard]] QString authorUrl() const;
    [[nodiscard]] Identity *authorIdentity() const;
    [[nodiscard]] QString blurhash() const;
    [[nodiscard]] QString description() const;
    [[nodiscard]] QString embedUrl() const;
    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;
    [[nodiscard]] QString html() const;
    [[nodiscard]] QString image() const;
    [[nodiscard]] QString providerName() const;
    [[nodiscard]] QString providerUrl() const;
    [[nodiscard]] QString title() const;
    [[nodiscard]] QUrl url() const;

private:
    /// Returns the new PreviewCardAuthor object if found.
    std::optional<QJsonObject> authorObject() const;

    QJsonObject m_card;
    AbstractAccount *m_account = nullptr;
};
