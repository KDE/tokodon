// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <KJob>
#include <QSaveFile>

class AbstractAccount;

class FileTransferJob final : public KJob
{
public:
    FileTransferJob(AbstractAccount *account, const QString &source, const QString &destination);
    void start() override;

    enum ExtraError {
        FileError = UserDefinedError,
    };

private:
    AbstractAccount *const m_account;
    QString m_source;
    QString m_destination;
    QScopedPointer<QSaveFile> m_temporaryFile;
};
