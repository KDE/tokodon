// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "utils/filehelper.h"

#include "account/abstractaccount.h"
#include "utils/filetransferjob.h"

#ifdef HAVE_KIO
#include <KIO/Job>
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

QString FileHelper::url(const QUrl &url) const
{
    return url.fileName().split(QLatin1Char('.')).last();
}

QString FileHelper::fileName(const QUrl &url) const
{
    return url.fileName();
}

void FileHelper::forceRefreshTextDocument(QQuickTextDocument *textDocument, QQuickItem *item)
{
    // HACK: Workaround bug QTBUG 93281, only applies to <6.7
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    connect(textDocument->textDocument(), SIGNAL(imagesLoaded()), item, SLOT(updateWholeDocument()));
#else
    Q_UNUSED(textDocument)
    Q_UNUSED(item)
#endif
}

#include "moc_filehelper.cpp"
