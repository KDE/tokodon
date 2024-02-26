// SPDX-FileCopyrightText: 2019 Black Hat <bhat@encom.eu.org>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QtQml>

class QClipboard;
class QImage;

/**
 * @brief Clipboard proxy
 */
class Clipboard : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool hasImage READ hasImage NOTIFY imageChanged)
    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)

public:
    explicit Clipboard(QObject *parent = nullptr);

    [[nodiscard]] bool hasImage() const;
    [[nodiscard]] QImage image() const;

    Q_INVOKABLE QString saveImage() const;

    Q_INVOKABLE void saveText(QString message);
    Q_INVOKABLE void copyImage(const QUrl &url);

private:
    QClipboard *m_clipboard;

Q_SIGNALS:
    void imageChanged();
};
