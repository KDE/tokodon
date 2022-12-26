// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QFile>
#include <QNetworkReply>

class TestReply : public QNetworkReply
{
public:
    TestReply(const QString &jsonFile, QObject *parent)
        : QNetworkReply(parent)
    {
        setError(NetworkError::NoError, QString());
        setFinished(true);

        apiResult.setFileName(QLatin1String(DATA_DIR) + QLatin1Char('/') + jsonFile);
        apiResult.open(QIODevice::ReadOnly);
    }

    virtual qint64 readData(char *data, qint64 maxSize) override
    {
        return apiResult.read(data, maxSize);
    }

    virtual void abort() override
    {
    }

    QFile apiResult;
};
