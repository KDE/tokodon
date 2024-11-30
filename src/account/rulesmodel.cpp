// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "account/rulesmodel.h"

#include "networkcontroller.h"

using namespace Qt::Literals::StringLiterals;

RulesModel::RulesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant RulesModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &announcement = m_rules[index.row()];

    switch (role) {
    case IdRole:
        return announcement.id;
    case TextRole:
        return announcement.text;
    default:
        return {};
    }
}

bool RulesModel::loading() const
{
    return m_loading;
}

void RulesModel::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

AbstractAccount *RulesModel::account() const
{
    return m_account;
}

void RulesModel::setAccount(AbstractAccount *account)
{
    if (m_account == account) {
        return;
    }
    m_account = account;
    Q_EMIT accountChanged();

    fill();
}

int RulesModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_rules.size();
}

QHash<int, QByteArray> RulesModel::roleNames() const
{
    return {{IdRole, "id"}, {TextRole, "text"}};
}

void RulesModel::fill()
{
    if (m_loading) {
        return;
    }
    setLoading(true);

    m_account->get(
        m_account->apiUrl(QStringLiteral("/api/v1/instance/rules")),
        false,
        this,
        [this](QNetworkReply *reply) {
            const auto doc = QJsonDocument::fromJson(reply->readAll());
            auto rules = doc.array().toVariantList();

            if (!rules.isEmpty()) {
                QList<Rule> fetchedRules;

                std::transform(rules.cbegin(), rules.cend(), std::back_inserter(fetchedRules), [=](const QVariant &value) -> auto {
                    return fromSourceData(value.toJsonObject());
                });
                beginInsertRows({}, m_rules.size(), m_rules.size() + fetchedRules.size() - 1);
                m_rules += fetchedRules;
                endInsertRows();
            }

            setLoading(false);
        },
        [=](QNetworkReply *reply) {
            Q_EMIT NetworkController::instance().networkErrorOccurred(reply->errorString());
        });
}

RulesModel::Rule RulesModel::fromSourceData(const QJsonObject &object) const
{
    Rule rule;
    rule.id = object["id"_L1].toString();
    rule.text = object["text"_L1].toString();

    return rule;
}

#include "moc_rulesmodel.cpp"
