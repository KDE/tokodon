// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "filetransferjob.h"

#include "account.h"
#include "tokodon_http_debug.h"

#include <KLocalizedString>

FileTransferJob::FileTransferJob(AbstractAccount *account, const QString &source, const QString &destination)
    : KJob()
    , m_account(account)
    , m_source(source)
    , m_destination(destination)
    , m_temporaryFile(new QSaveFile(QUrl(destination).toLocalFile()))
{
}

void FileTransferJob::start()
{
    if (auto account = qobject_cast<Account *>(m_account); account) {
        auto qnam = account->qnam();

        auto reply = qnam->get(QNetworkRequest(QUrl(m_source)));

        setTotalAmount(Unit::Files, 1);
        if (!m_temporaryFile->isReadable() && !m_temporaryFile->open(QIODevice::WriteOnly)) {
            qCWarning(TOKODON_HTTP) << "Couldn't open the temporary file" << m_temporaryFile->fileName() << "for writing" << m_temporaryFile->errorString();
            setError(FileError);
            setErrorText(i18n("Could not open the temporary download file"));
            emitResult();
            return;
        }
        connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesReceived, qint64 bytesTotal) {
            if (bytesTotal != -1) {
                setTotalAmount(Unit::Bytes, bytesTotal);
            }
            setProcessedAmount(Unit::Bytes, bytesReceived);
        });

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                setError(FileError);
                setErrorText(reply->errorString());
                emitResult();
                return;
            }
            if (!m_temporaryFile->commit()) {
                qCWarning(TOKODON_HTTP) << "errror when saving";
            }
            reply->deleteLater();
            emitResult();
        });

        connect(reply, &QIODevice::readyRead, this, [this, reply] {
            auto bytes = reply->read(reply->bytesAvailable());
            if (!bytes.isEmpty()) {
                m_temporaryFile->write(bytes);
            } else {
                qCWarning(TOKODON_HTTP) << "Unexpected empty chunk when downloading from" << reply->url() << "to" << m_temporaryFile->fileName();
            }
        });

        connect(reply, &QNetworkReply::metaDataChanged, this, [this, reply] {
            auto sizeHeader = reply->header(QNetworkRequest::ContentLengthHeader);
            if (sizeHeader.isValid()) {
                auto targetSize = sizeHeader.toLongLong();
                if (targetSize != -1) {
                    if (!m_temporaryFile->resize(targetSize)) {
                        qCWarning(TOKODON_HTTP) << "Failed to allocate" << targetSize << "bytes for" << m_temporaryFile->fileName();
                        setError(FileError);
                        setErrorText(i18n("Could not reserve disk space for download"));
                        emitResult();
                    }
                }
            }
        });

        Q_EMIT description(this,
                           i18nc("Job heading, like 'Copying'", "Downloading"),
                           {i18nc("The URL being downloaded/uploaded", "Source"), m_source},
                           {i18nc("The location being downloaded to", "Destination"), m_destination});
    }
}
