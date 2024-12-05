// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QJsonObject>
#include <QQmlEngine>

class Post;

/**
 * @brief Post's attachment object.
 * @see Post
 */
// TODO: make it possible to fetch the images with a Qml image provider.
// TODO: use getter and setter
class Attachment : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Access via Post")

    Q_PROPERTY(QString id MEMBER m_id CONSTANT)
    Q_PROPERTY(AttachmentType attachmentType MEMBER m_type CONSTANT)
    Q_PROPERTY(int type READ isVideo CONSTANT)
    Q_PROPERTY(QString previewUrl MEMBER m_preview_url CONSTANT)
    Q_PROPERTY(QString source MEMBER m_url CONSTANT)
    Q_PROPERTY(QString remoteUrl MEMBER m_remote_url CONSTANT)
    Q_PROPERTY(QString caption READ description CONSTANT)
    Q_PROPERTY(QUrl tempSource READ tempSource CONSTANT)
    Q_PROPERTY(int sourceWidth MEMBER m_sourceWidth CONSTANT)
    Q_PROPERTY(int sourceHeight MEMBER m_sourceHeight CONSTANT)
    Q_PROPERTY(double focusX READ focusX WRITE setFocusX NOTIFY focusXChanged)
    Q_PROPERTY(double focusY READ focusY WRITE setFocusY NOTIFY focusYChanged)

public:
    explicit Attachment(QObject *parent = nullptr);
    explicit Attachment(const QJsonObject &object, QObject *parent = nullptr);

    enum AttachmentType {
        Unknown,
        Image,
        GifV,
        Video,
    };
    Q_ENUM(AttachmentType);

    Post *m_parent = nullptr;

    QString m_id;
    AttachmentType m_type = AttachmentType::Unknown;
    QString m_preview_url;
    QString m_url;
    QString m_remote_url;
    int m_sourceWidth = -1;
    int m_sourceHeight = -1;

    [[nodiscard]] QString id() const;

    void setDescription(const QString &description);
    [[nodiscard]] QString description() const;

    /**
     * @brief Used exclusively in Maximize component to tell it whether or not an attachment is a video
     */
    [[nodiscard]] int isVideo() const;

    [[nodiscard]] QUrl tempSource() const;

    [[nodiscard]] double focusX() const;
    void setFocusX(double value);

    [[nodiscard]] double focusY() const;
    void setFocusY(double value);

    /**
     * @brief Copies the attachment image data to the clipboard.
     * @note We don't do this on the QML side because we have to do a network request. We might as well do it all in C++.
     */
    Q_INVOKABLE void copyToClipboard();

Q_SIGNALS:
    void focusXChanged();
    void focusYChanged();

private:
    void fromJson(const QJsonObject &object);

    QString m_description;
    QString m_blurhash;
    double m_focusX = 0.0f;
    double m_focusY = 0.0f;
};
