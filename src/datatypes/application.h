// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QJsonObject>

class Application
{
    Q_GADGET

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QUrl website READ website)

public:
    Application() = default;
    explicit Application(QJsonObject application);

    [[nodiscard]] QString name() const;
    [[nodiscard]] QUrl website() const;

private:
    QJsonObject m_application;
};
