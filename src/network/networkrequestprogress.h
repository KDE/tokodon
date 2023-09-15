// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

class QNetworkReply;

class NetworkRequestProgress : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QNetworkReply *reply READ reply WRITE setReply NOTIFY replyChanged)

    Q_PROPERTY(bool uploading READ uploading NOTIFY progressChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)

public:
    explicit NetworkRequestProgress(QObject *parent = nullptr);

    QNetworkReply *reply() const;
    void setReply(QNetworkReply *reply);

    bool uploading() const;

    int progress() const;
    void setProgress(int progress);

Q_SIGNALS:
    void replyChanged();
    void progressChanged();

private:
    QNetworkReply *m_reply = nullptr;
    int m_progress = 0;
};
