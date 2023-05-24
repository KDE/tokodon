// SPDX-FileCopyrightText: 2019 Harald Sitter <sitter@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "messagefiltercontainer.h"

// Extra filter rigging. We don't want to leak secrets via qdebug, so install
// a message handler which does nothing more than replace secrets in debug
// messages with placeholders.
// This is used as a global static (since message handlers are meant to be
// static) and is slightly synchronizing across threads WRT the filter hash.

QMessageFilterContainer::QMessageFilterContainer()
{
    qInstallMessageHandler(nullptr);

    handler = qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        s_messageFilter->handler(type, context, s_messageFilter->filter(msg));
    });
}

QMessageFilterContainer::~QMessageFilterContainer()
{
    qInstallMessageHandler(handler);
}

void QMessageFilterContainer::insert(const QString &needle, const QString &replace)
{
    if (needle.isEmpty()) {
        return;
    }

    QWriteLocker locker(&lock);
    filters[needle] = replace;
}

QString QMessageFilterContainer::filter(const QString &msg)
{
    QReadLocker locker(&lock);
    QString filteredMsg = msg;
    for (auto it = filters.constBegin(); it != filters.constEnd(); ++it) {
        filteredMsg.replace(it.key(), it.value());
    }
    return filteredMsg;
}

void QMessageFilterContainer::clear()
{
    QWriteLocker locker(&lock);
    filters.clear();
}
