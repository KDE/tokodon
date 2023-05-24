// SPDX-FileCopyrightText: 2019 Harald Sitter <sitter@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QHash>
#include <QReadWriteLock>
#include <QString>
#include <QtMessageHandler>

struct QMessageFilterContainer {
    QMessageFilterContainer();
    ~QMessageFilterContainer();
    void insert(const QString &needle, const QString &replace);
    void clear();

    QString filter(const QString &msg);

    // Message handler is called across threads. Synchronize for good measure.
    QReadWriteLock lock;
    QtMessageHandler handler;

private:
    QHash<QString, QString> filters;
};

Q_GLOBAL_STATIC(QMessageFilterContainer, s_messageFilter)
