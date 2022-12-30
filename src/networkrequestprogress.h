// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>

class QNetworkReply;

class NetworkRequestProgress : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QNetworkReply *reply READ reply WRITE setReply NOTIFY replyChanged)

    Q_PROPERTY(bool uploading READ uploading NOTIFY uploadingChanged NOTIFY replyChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)

public:
    NetworkRequestProgress(QObject *parent = nullptr);

    QNetworkReply *reply() const;
    void setReply(QNetworkReply *reply);

    bool uploading() const;

    int progress() const;
    void setProgress(int progress);

Q_SIGNALS:
    void replyChanged();
    void uploadingChanged();
    void progressChanged();

private:
    QNetworkReply *m_reply = nullptr;
    int m_progress = 0;
};
