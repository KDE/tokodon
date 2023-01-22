// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "networkrequestprogress.h"
#include <QNetworkReply>

NetworkRequestProgress::NetworkRequestProgress(QObject *parent)
    : QObject(parent)
{
}

QNetworkReply *NetworkRequestProgress::reply() const
{
    return m_reply;
}

void NetworkRequestProgress::setReply(QNetworkReply *reply)
{
    if (reply == m_reply) {
        return;
    }
    m_reply = reply;
    Q_EMIT replyChanged();

    if (m_reply) {
        connect(reply, &QNetworkReply::uploadProgress, this, [this](qint64 bytesSent, qint64 bytesTotal) {
            double progress = static_cast<double>(bytesSent) / static_cast<double>(bytesTotal) * 100.0;
            if (bytesTotal != 0) {
                setProgress(progress);
            } else {
                Q_EMIT uploadingChanged();
            }
        });
        connect(reply, &QNetworkReply::finished, this, [this] {
            m_reply = nullptr;
            Q_EMIT uploadingChanged();
        });
        setProgress(reply->isFinished() ? 100 : 0);
    }
}

bool NetworkRequestProgress::uploading() const
{
    return m_reply && !m_reply->isFinished();
}

int NetworkRequestProgress::progress() const
{
    return m_progress;
}
void NetworkRequestProgress::setProgress(int progress)
{
    if (m_progress == progress) {
        return;
    }
    m_progress = progress;
    Q_EMIT progressChanged();
}
