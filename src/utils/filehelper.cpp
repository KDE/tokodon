// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "utils/filehelper.h"

#include "account/abstractaccount.h"
#include "utils/filetransferjob.h"

#ifdef HAVE_KIO
#include <KIO/JobTracker>
#include <KJobTrackerInterface>
#endif

FileHelper::FileHelper(QObject *parent)
    : QObject(parent)
{
}

FileHelper::~FileHelper() = default;

void FileHelper::downloadFile(AbstractAccount *account, const QString &url, const QString &destination) const
{
    auto job = new FileTransferJob(account, url, destination);
#ifdef HAVE_KIO
    KIO::getJobTracker()->registerJob(job);
#endif
    job->start();
}

QString FileHelper::fileName(const QUrl &url) const
{
    return url.fileName();
}

#include "moc_filehelper.cpp"
