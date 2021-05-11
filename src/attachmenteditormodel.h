// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QAbstractListModel>
#include <memory>

#include "post.h"

class AttachmentEditorModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Post *post READ post WRITE setPost NOTIFY postChanged)
public:
    explicit AttachmentEditorModel(QObject *parent = nullptr);

    enum ExtraRole {
        Preview = Qt::UserRole + 1,
        Description,
    };

    Post *post() const;
    void setPost(Post *post);

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void postChanged();

private:
    Post *m_scratch;
};

