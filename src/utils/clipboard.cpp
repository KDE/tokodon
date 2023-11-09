/**
 * SPDX-FileCopyrightText: 2019 Black Hat <bhat@encom.eu.org>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "clipboard.h"

#include "abstractaccount.h"
#include "accountmanager.h"

#include <QClipboard>
#include <QGuiApplication>

Clipboard::Clipboard(QObject *parent)
    : QObject(parent)
    , m_clipboard(QGuiApplication::clipboard())
{
    connect(m_clipboard, &QClipboard::changed, this, &Clipboard::imageChanged);
}

bool Clipboard::hasImage() const
{
    return !image().isNull();
}

QImage Clipboard::image() const
{
    return m_clipboard->image();
}

QString Clipboard::saveImage() const
{
    const QString imageDir(QStringLiteral("%1/screenshots").arg(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)));
    const QString localPath = QStringLiteral("file://%1/%2.png").arg(imageDir, QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd-hh-mm-ss")));

    if (!QDir().exists(imageDir)) {
        QDir().mkdir(imageDir);
    }

    QUrl url(localPath);
    if (!url.isLocalFile()) {
        return {};
    }
    auto image = this->image();

    if (image.isNull()) {
        return {};
    }

    if (image.save(url.toLocalFile())) {
        return localPath;
    } else {
        return {};
    }
}

void Clipboard::saveText(QString message)
{
    const static QRegularExpression re(QStringLiteral("<[^>]*>"));
    auto *mineData = new QMimeData; // ownership is transferred to clipboard
    mineData->setHtml(message);
    mineData->setText(message.replace(re, QStringLiteral("")));
    m_clipboard->setMimeData(mineData);
}

void Clipboard::copyImage(const QUrl &url)
{
    AccountManager::instance().selectedAccount()->get(
        url,
        false,
        this,
        [this](QNetworkReply *reply) {
            QImage image;
            image.loadFromData(reply->readAll());

            m_clipboard->setImage(image);
        },
        nullptr);
}

#include "moc_clipboard.cpp"