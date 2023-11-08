// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "windowcontroller.h"

#include <KSharedConfig>
#include <KWindowConfig>

void WindowController::setWindow(QQuickWindow *window)
{
    Q_ASSERT(window != nullptr);
    m_window = window;
}

void WindowController::restoreGeometry()
{
    auto stateConfig = KSharedConfig::openStateConfig();
    auto windowGroup = stateConfig->group(QStringLiteral("Window"));

    KWindowConfig::restoreWindowPosition(m_window, windowGroup);
    KWindowConfig::restoreWindowSize(m_window, windowGroup);

    stateConfig->sync();
}

void WindowController::saveGeometry()
{
    auto stateConfig = KSharedConfig::openStateConfig();
    auto windowGroup = stateConfig->group(QStringLiteral("Window"));

    KWindowConfig::saveWindowPosition(m_window, windowGroup);
    KWindowConfig::saveWindowSize(m_window, windowGroup);

    stateConfig->sync();
}

#include "moc_windowcontroller.cpp"