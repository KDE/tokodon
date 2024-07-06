// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "timeline/post.h"

class QTimer;
class AbstractAccount;

class AttachmentEditorModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit AttachmentEditorModel(QObject *parent, AbstractAccount *account);

    enum ExtraRole { PreviewRole = Qt::UserRole + 1, DescriptionRole, FocalXRole, FocalYRole };

    int count() const;

    Q_INVOKABLE int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    const QList<Attachment *> &attachments() const;

    void copyFromOther(AttachmentEditorModel *other);

public Q_SLOTS:
    QNetworkReply *append(const QString &fileName);
    QNetworkReply *appendData(QVariant data);
    void appendExisting(Attachment *attachment);
    void removeAttachment(int row);
    void setDescription(int row, const QString &description);
    void setFocusPoint(int row, double x, double y);
    bool isLocalFile(const QString &fileName);

Q_SIGNALS:
    void postChanged();
    void countChanged();

private:
    QList<Attachment *> m_attachments;
    QHash<QString, QTimer *> m_updateTimers;
    AbstractAccount *m_account;

    QTemporaryDir m_saveDir;
};
