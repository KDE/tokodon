// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QtQml>

class AbstractAccount;

/**
 * @brief Deals with downloading files, and misc operations not natively supported in QML.
 */
class FileHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit FileHelper(QObject *parent = nullptr);
    ~FileHelper() override;

    /**
     * @brief Downloads a file.
     * @param account The account to handle the download.
     * @param url The url to download.
     * @param destination The path on disk where the file will end up when finished.
     * @note Uses KIO when available.
     */
    Q_INVOKABLE void downloadFile(AbstractAccount *account, const QString &url, const QString &destination) const;

    // TODO: I think this can be done natively using JavaScript's URL object?
    /**
     * @return The filename portion of a URL.
     */
    Q_INVOKABLE [[nodiscard]] QString fileName(const QUrl &url) const;
};
