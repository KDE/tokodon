// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

class AbstractAccount;

class FileHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit FileHelper(QObject *parent = nullptr);
    ~FileHelper() override;

    Q_INVOKABLE void downloadFile(AbstractAccount *account, const QString &url, const QString &destination) const;
    Q_INVOKABLE QString url(const QUrl &url) const;
    Q_INVOKABLE QString fileName(const QUrl &url) const;
};
