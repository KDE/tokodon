// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "attachment.h"

using namespace Qt::StringLiterals;

static QMap<QString, Attachment::AttachmentType> stringToAttachmentType = {
    {QStringLiteral("image"), Attachment::AttachmentType::Image},
    {QStringLiteral("gifv"), Attachment::AttachmentType::GifV},
    {QStringLiteral("video"), Attachment::AttachmentType::Video},
    {QStringLiteral("unknown"), Attachment::AttachmentType::Unknown},
};

Attachment::Attachment(QObject *parent)
    : QObject(parent)
{
}

Attachment::Attachment(const QJsonObject &obj, QObject *parent)
    : QObject(parent)
{
    fromJson(obj);
}

void Attachment::fromJson(const QJsonObject &obj)
{
    if (!obj.contains("type"_L1)) {
        m_type = Unknown;
        return;
    }

    m_id = obj["id"_L1].toString();
    m_url = obj["url"_L1].toString();
    m_preview_url = obj["preview_url"_L1].toString();
    m_remote_url = obj["remote_url"_L1].toString();

    setDescription(obj["description"_L1].toString());
    m_blurhash = obj["blurhash"_L1].toString();
    m_sourceHeight = obj["meta"_L1].toObject()["original"_L1].toObject()["height"_L1].toInt();
    m_sourceWidth = obj["meta"_L1].toObject()["original"_L1].toObject()["width"_L1].toInt();

    // determine type if we can
    const auto type = obj["type"_L1].toString();
    if (stringToAttachmentType.contains(type)) {
        m_type = stringToAttachmentType[type];
    }

    if (!m_remote_url.isEmpty()) {
        const auto mimeType = QMimeDatabase().mimeTypeForFile(m_remote_url);
        if (m_type == AttachmentType::Unknown && mimeType.name().contains("image"_L1)) {
            m_type = AttachmentType::Image;
        }
    }

    if (obj.contains("meta"_L1) && obj["meta"_L1].toObject().contains("focus"_L1)) {
        m_focusX = obj["meta"_L1].toObject()["focus"_L1].toObject()["x"_L1].toDouble();
        m_focusY = obj["meta"_L1].toObject()["focus"_L1].toObject()["y"_L1].toDouble();
    }
}

QString Attachment::description() const
{
    return m_description;
}

void Attachment::setDescription(const QString &description)
{
    m_description = description;
}

QString Attachment::id() const
{
    return m_id;
}

int Attachment::isVideo() const
{
    if (m_type == AttachmentType::GifV || m_type == AttachmentType::Video) {
        return 1;
    }

    return 0;
}

QString Attachment::tempSource() const
{
    return QStringLiteral("image://blurhash/%1").arg(m_blurhash);
}

double Attachment::focusX() const
{
    return m_focusX;
}

void Attachment::setFocusX(double value)
{
    if (value != m_focusX) {
        m_focusX = value;
        Q_EMIT focusXChanged();
    }
}

double Attachment::focusY() const
{
    return m_focusY;
}

void Attachment::setFocusY(double value)
{
    if (value != m_focusY) {
        m_focusY = value;
        Q_EMIT focusYChanged();
    }
}

#include "moc_attachment.cpp"