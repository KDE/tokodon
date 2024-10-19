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

    qint64 readData(char *data, qint64 maxSize) override
    {
        return apiResult.read(data, maxSize);
    }

    bool seek(const qint64 pos) override
    {
        return apiResult.seek(pos);
    }

    void abort() override
    {
    }

    void setRawHeader(const QByteArray &headerName, const QByteArray &value)
    {
        QNetworkReply::setRawHeader(headerName, value);
    }

    QFile apiResult;
};
