// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "languagemodel.h"

#include <QDebug>

RawLanguageModel::RawLanguageModel(QObject *parent)
{
    auto locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);

    for (const auto &locale : locales) {
        if (!m_languages.contains(locale.language()) && locale != QLocale::c()) {
            m_languages.push_back(locale.language());
            m_iso639codes.push_back(locale.name().split('_').first());
        }
    }

    for (const auto &language : QLocale().uiLanguages()) {
        if (!language.contains('-')) {
            m_preferredLanguages.push_back(language);
        }
    }
}

QVariant RawLanguageModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case CustomRoles::NameRole:
        return QLocale::languageToString(m_languages[index.row()]);
    case CustomRoles::CodeRole:
        return m_iso639codes[index.row()];
    case CustomRoles::PreferredRole:
        return m_preferredLanguages.contains(m_iso639codes[index.row()]);
    default:
        return {};
    }
}

int RawLanguageModel::rowCount(const QModelIndex &parent) const
{
    return m_languages.count();
}

QHash<int, QByteArray> RawLanguageModel::roleNames() const
{
    return {{CustomRoles::NameRole, "name"}, {CustomRoles::CodeRole, "code"}, {CustomRoles::PreferredRole, "preferred"}};
}

QString RawLanguageModel::getCode(const int index) const
{
    return m_iso639codes[index];
}

LanguageModel::LanguageModel(QObject *parent)
{
    m_model = new RawLanguageModel(this);

    setSourceModel(m_model);
    sort(0, Qt::AscendingOrder);
}

bool LanguageModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const bool preferred = sourceModel()->data(left, RawLanguageModel::CustomRoles::PreferredRole).toBool();

    if (preferred) {
        return true;
    } else {
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

QString LanguageModel::getCode(const int row) const
{
    return m_model->getCode(mapToSource(index(row, 0)).row());
}
