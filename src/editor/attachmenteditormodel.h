// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QAbstractListModel>
#include <memory>

#include "timeline/post.h"
#include "posteditorbackend.h"

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
    const QVector<Attachment *> &attachments() const;

public Q_SLOTS:
    QNetworkReply *append(const QString &fileName);
    void appendExisting(Attachment *attachment);
    void removeAttachment(int row);
    void setDescription(int row, const QString &description);
    void setFocusPoint(int row, double x, double y);

Q_SIGNALS:
    void postChanged();
    void countChanged();

private:
    QVector<Attachment *> m_attachments;
    QHash<QString, QTimer *> m_updateTimers;
    AbstractAccount *m_account;
};
