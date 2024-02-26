// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "account/abstractaccount.h"

/**
 * @brief Fetches server rules.
 */
class RulesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(AbstractAccount *account READ account WRITE setAccount NOTIFY accountChanged)

public:
    /**
     * @brief Custom roles for this model.
     */
    enum CustomRoles {
        IdRole = Qt::UserRole, /** ID of the rule. */
        TextRole, /** Text content of the rule. */
    };

    explicit RulesModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool loading);

    AbstractAccount *account() const;
    void setAccount(AbstractAccount *account);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void loadingChanged();
    void accountChanged();

private:
    void fill();

    struct Rule {
        QString id, text;
    };

    QList<Rule> m_rules;
    bool m_loading = false;
    AbstractAccount *m_account = nullptr;
    Rule fromSourceData(const QJsonObject &object) const;
};
