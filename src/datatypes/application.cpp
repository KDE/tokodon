// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "datatypes/application.h"

Application::Application(QJsonObject application)
    : m_application(application)
{
}

QString Application::name() const
{
    return m_application[QLatin1String("name")].toString();
}

QUrl Application::website() const
{
    return QUrl::fromUserInput(m_application[QLatin1String("website")].toString());
}
