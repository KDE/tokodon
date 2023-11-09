// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QQuickWindow>

class Identity;
class Post;

class WindowController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    void setWindow(QQuickWindow *window);

    void restoreGeometry();
    Q_INVOKABLE void saveGeometry();

private:
    QQuickWindow *m_window = nullptr;
};
