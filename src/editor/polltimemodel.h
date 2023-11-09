// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtQml>

struct Time {
    QString name;
    int time = 0;
};

class PollTimeModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum CustomRoles { TextRole = Qt::UserRole + 1, TimeRole };

    explicit PollTimeModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int getTime(int index);

private:
    QList<Time> m_times;
};