// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#include "attachmenteditormodel.h"
#include "account.h"

AttachmentEditorModel::AttachmentEditorModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_scratch(nullptr)
{
}

Post *AttachmentEditorModel::post() const
{
    return m_scratch;
}

void AttachmentEditorModel::setPost(Post *post)
{
    if (post == m_scratch) {
        return;
    }
    if (m_scratch) {
        disconnect(m_scratch->m_parent, nullptr, this, nullptr);
    }
    beginResetModel();
    m_scratch = post;
    endResetModel();
    if (m_scratch) {
        connect(m_scratch, &Post::attachmentUploaded, this, [this] {
            qDebug() << "Uploaded 2";
            // TODO beginInsertModel
            beginResetModel();
            endResetModel();
            Q_EMIT countChanged();
        });
    }

    Q_EMIT postChanged();
    Q_EMIT countChanged();
}

int AttachmentEditorModel::rowCount(const QModelIndex &) const
{
    if (!m_scratch) {
        return 0;
    }
    return m_scratch->m_attachments.size();
}

int AttachmentEditorModel::count() const
{
    if (!m_scratch) {
        return 0;
    }
    return m_scratch->m_attachments.size();
}

QVariant AttachmentEditorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    int row = index.row();
    auto att = m_scratch->m_attachments[row];

    switch (role) {
    case Preview:
        qDebug() << att->m_preview_url;
        return att->m_preview_url;
    case Description:
        return att->m_description;
    }

    return {};
}

bool AttachmentEditorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const int row = index.row();

    if (role != Qt::EditRole)
        return false;

    if (row != Description)
        return false;

    auto att = m_scratch->m_attachments[row];
    att->setDescription(value.toString());

    return true;
}

QHash<int, QByteArray> AttachmentEditorModel::roleNames() const
{
    return {{Preview, QByteArrayLiteral("preview")}, {Description, QByteArrayLiteral("description")}, {Qt::DisplayRole, QByteArrayLiteral("display")}};
}
