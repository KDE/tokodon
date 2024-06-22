// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QQuickWindow>

class Identity;
class Post;

/**
 * @brief Handles some miscenalleous window management stuffs.
 */
class WindowController final : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    /**
     * Sets the @p window to save/restore information about.
     */
    void setWindow(QQuickWindow *window);

    /**
     * @brief Restores the window's geometry to disk.
     * @note Only the window size is restored on Wayland.
     */
    void restoreGeometry();

    /**
     * @brief Saves the window's geometry to disk.
     */
    Q_INVOKABLE void saveGeometry();

private:
    QQuickWindow *m_window = nullptr;
};
