// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "threadmodel.h"
#include <KLocalizedString>

ThreadModel::ThreadModel(AccountManager *manager, const QString &timelineName, QObject *parent)
    : TimelineModel(parent)
{
    setName(timelineName);
    setAccountManager(manager);
}

QString ThreadModel::displayName() const
{
    return i18nc("@title", "Thread");
}

void ThreadModel::fillTimeline(QString)
{
    m_fetching = true;

    m_account->fetchThread(m_timeline_name, [=](const QList<std::shared_ptr<Post>> posts) {
        qDebug() << "Got" << posts.size() << "posts";

        m_timeline = posts;

        beginInsertRows(QModelIndex(), 0, m_timeline.size() - 1);
        endInsertRows();
    });
}

bool ThreadModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return false;
}
