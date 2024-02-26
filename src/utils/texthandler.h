/**
 * SPDX-FileCopyrightText: 2023 Janet Black
 * SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QtQml>

#include "identity.h"

/**
 * Clipboard proxy
 */
class TextHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit TextHandler(QObject *parent = nullptr);
    ~TextHandler() override;

    static QString fixBidirectionality(const QString &html);

    /// Parses a HTML body and returns a processed body and a list of tags respectively.
    /// The returned body does not have the tags included and is cleaned up.
    static QPair<QString, QList<QString>> removeStandaloneTags(QString contentHtml);
};
