// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "editor/languagemodel.h"

using namespace Qt::Literals::StringLiterals;

RawLanguageModel::RawLanguageModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);

    for (const auto &locale : locales) {
        if (!m_languages.contains(locale.language()) && locale != QLocale::c()) {
            m_languages.push_back(locale.language());
            m_iso639codes.push_back(QLocale::languageToCode(locale.language(), QLocale::ISO639));
        }
    }

    for (const auto &language : QLocale().uiLanguages()) {
        if (!language.contains('-'_L1)) {
            m_preferredLanguages.push_back(language);
        }
    }
}

QVariant RawLanguageModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case CustomRoles::NameRole: {
        // Use the native language name if it exists
        if (const QString nativeName = QLocale(m_languages[index.row()]).nativeLanguageName(); !nativeName.isEmpty()) {
            return nativeName;
        } else if (const QString languageString = QLocale::languageToString(m_languages[index.row()]); !languageString.isEmpty()) {
            return languageString;
        } else {
            return m_iso639codes[index.row()];
        }
    }
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
    Q_UNUSED(parent);

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

QModelIndex RawLanguageModel::indexOfValue(const QString &code)
{
    const auto it = std::ranges::find(std::as_const(m_iso639codes), code);
    if (it != m_iso639codes.cend()) {
        return index(std::distance(m_iso639codes.cbegin(), it), 0);
    } else {
        return {};
    }
}

#include "moc_languagemodel.cpp"
