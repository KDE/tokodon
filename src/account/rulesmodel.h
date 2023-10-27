// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

/// Fetches server rules
class RulesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    /// Custom roles for this model
    enum CustomRoles {
        IdRole = Qt::UserRole, ///< ID of the rule
        TextRole, ///< Text content of the rule
    };

    explicit RulesModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void loadingChanged();

private:
    void fill();

    struct Rule {
        QString id, text;
    };

    QList<Rule> m_rules;
    bool m_loading = false;
    Rule fromSourceData(const QJsonObject &object) const;
};
