/**
 * SPDX-FileCopyrightText: 2023 Janet Black
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QtQml>

/**
 * Clipboard proxy
 */
class TextPreprocessing : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit TextPreprocessing(QObject *parent = nullptr);
    ~TextPreprocessing();

    Q_INVOKABLE QString preprocessHTML(const QString &html, const QColor &linkColor);
};
