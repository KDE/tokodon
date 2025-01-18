// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "editor/attachmenteditormodel.h"

#include <QJsonDocument>

#include "account/account.h"

AttachmentEditorModel::AttachmentEditorModel(QObject *parent, AbstractAccount *account)
    : QAbstractListModel(parent)
    , m_account(account)
{
}

int AttachmentEditorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_attachments.size();
}

int AttachmentEditorModel::count() const
{
    return rowCount({});
}

QVariant AttachmentEditorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    const int row = index.row();
    const auto &attachment = m_attachments[row];

    switch (role) {
    case PreviewRole:
        return attachment->m_preview_url;
    case DescriptionRole:
        return attachment->description();
    case FocalXRole:
        return attachment->focusX();
    case FocalYRole:
        return attachment->focusY();
    }

    return {};
}

QHash<int, QByteArray> AttachmentEditorModel::roleNames() const
{
    return {
        {PreviewRole, QByteArrayLiteral("preview")},
        {DescriptionRole, QByteArrayLiteral("description")},
        {FocalXRole, QByteArrayLiteral("focalX")},
        {FocalYRole, QByteArrayLiteral("focalY")},
    };
}

QNetworkReply *AttachmentEditorModel::append(const QString &filename)
{
    if (rowCount({}) >= m_account->maxMediaAttachments()) {
        return nullptr;
    }

    QString localFilename = filename;
    localFilename.remove(QStringLiteral("file://"));

    return m_account->upload(QUrl::fromLocalFile(localFilename), [this](QNetworkReply *reply) {
        const auto doc = QJsonDocument::fromJson(reply->readAll());

        if (!doc.isObject()) {
            return;
        }

        beginInsertRows({}, m_attachments.count(), m_attachments.count());
        m_attachments.append(new Attachment{doc.object(), this});
        endInsertRows();
        Q_EMIT countChanged();
    });
}

QNetworkReply *AttachmentEditorModel::appendData(QVariant data)
{
    const auto image = data.value<QImage>();

    const QString tempPath = m_saveDir.filePath(QStringLiteral("%1.png").arg(QUuid::createUuid().toString()));
    if (image.save(tempPath, "PNG")) {
        return append(tempPath);
    }

    return nullptr;
}

void AttachmentEditorModel::appendExisting(Attachment *attachment)
{
    beginInsertRows({}, m_attachments.count(), m_attachments.count());
    m_attachments.append(attachment);
    endInsertRows();
    Q_EMIT countChanged();
}

void AttachmentEditorModel::removeAttachment(int row)
{
    beginRemoveRows({}, row, row);
    m_attachments.removeAt(row);
    endRemoveRows();
    Q_EMIT countChanged();
}

void AttachmentEditorModel::setDescription(int row, const QString &description)
{
    auto &attachment = m_attachments[row];
    const auto id = attachment->id();
    attachment->setDescription(description);

    const auto attachementUrl = m_account->apiUrl(QStringLiteral("/api/v1/media/%1").arg(id));
    const QJsonObject obj{
        {QStringLiteral("description"), description},
    };
    const QJsonDocument doc(obj);
    m_account->put(attachementUrl, doc, true, this, nullptr);
    Q_EMIT dataChanged(index(row, 0), index(row, 0), {DescriptionRole});
}

void AttachmentEditorModel::setFocusPoint(int row, double x, double y)
{
    auto &attachment = m_attachments[row];
    const auto id = attachment->id();
    attachment->setFocusX(x);
    attachment->setFocusY(y);

    const auto attachementUrl = m_account->apiUrl(QStringLiteral("/api/v1/media/%1").arg(id));
    const QJsonObject obj{
        {QStringLiteral("focus"), QStringLiteral("%1,%2").arg(x).arg(y)},
    };
    const QJsonDocument doc(obj);
    m_account->put(attachementUrl, doc, true, this, nullptr);
    Q_EMIT dataChanged(index(row, 0), index(row, 0), {FocalXRole, FocalYRole});
}

const QList<Attachment *> &AttachmentEditorModel::attachments() const
{
    return m_attachments;
}

void AttachmentEditorModel::copyFromOther(AttachmentEditorModel *other)
{
    beginResetModel();
    m_attachments = other->m_attachments;
    endResetModel();
    m_account = other->m_account;
}

void AttachmentEditorModel::copyFromArray(const QJsonArray &array)
{
    beginResetModel();
    m_attachments.clear();
    std::ranges::transform(std::as_const(array), std::back_inserter(m_attachments), [this](const QJsonValue &value) {
        return new Attachment{value.toObject(), this};
    });
    endResetModel();
}

bool AttachmentEditorModel::isLocalFile(const QString &fileName)
{
    QString localFilename = fileName;
    localFilename.remove(QStringLiteral("file://"));

    QFileInfo info(localFilename);
    return info.exists();
}

bool AttachmentEditorModel::isAltTextComplete() const
{
    for (auto &attachment : m_attachments) {
        if (attachment->description().isEmpty()) {
            return false;
        }
    }
    return true;
}

#include "moc_attachmenteditormodel.cpp"
