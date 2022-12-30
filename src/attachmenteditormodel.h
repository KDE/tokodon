// SPDX-FileCopyrightText: 2021 kaniini <https://git.pleroma.social/kaniini>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QAbstractListModel>
#include <memory>

#include "post.h"
#include "posteditorbackend.h"

class QTimer;
class AbstractAccount;

class AttachmentEditorModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit AttachmentEditorModel(QObject *parent, AbstractAccount *account);

    enum ExtraRole {
        Preview = Qt::UserRole + 1,
        Description,
    };

    int count() const;

    Q_INVOKABLE int rowCount(const QModelIndex &parent = {}) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    std::vector<Attachment> &attachments() const;

public Q_SLOTS:
    QNetworkReply *append(const QUrl &fileName);

Q_SIGNALS:
    void postChanged();
    void countChanged();

private:
    QVector<Attachment> m_attachments;
    QHash<QString, QTimer *> m_updateTimers;
    AbstractAccount *m_account;
};
