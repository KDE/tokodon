// SPDX-FileCopyrightText: 2019 Harald Sitter <sitter@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QHash>
#include <QReadWriteLock>

struct QMessageFilterContainer {
    QMessageFilterContainer();
    ~QMessageFilterContainer();
    void insert(const QString &needle, const QString &replace);
    void clear();

    QString filter(const QString &msg);

    static QMessageFilterContainer *self();

private:
    // Message handler is called across threads. Synchronize for good measure.
    QReadWriteLock lock;
    QtMessageHandler handler;
    QHash<QString, QString> filters;
};
