// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "datatypes/application.h"

Application::Application(QJsonObject application)
    : m_application(application)
{
}

bool Application::isValid() const
{
    return !m_application.isEmpty();
}

QString Application::name() const
{
    return m_application[QLatin1String("name")].toString();
}

QUrl Application::website() const
{
    return QUrl::fromUserInput(m_application[QLatin1String("website")].toString());
}

#include "moc_application.cpp"
