// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtQml>

class RawLanguageModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum CustomRoles { NameRole = Qt::UserRole + 1, CodeRole, PreferredRole };

    explicit RawLanguageModel(QObject *parent = nullptr);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE [[nodiscard]] QString getCode(int index) const;
    Q_INVOKABLE QModelIndex indexOfValue(const QString &code);

private:
    QList<QLocale::Language> m_languages;
    QList<QString> m_iso639codes;
    QList<QString> m_preferredLanguages;
};