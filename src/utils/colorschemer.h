// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QQmlEngine>

class QAbstractItemModel;
class KColorSchemeManager;

class ColorSchemer : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QAbstractItemModel *model READ model CONSTANT)

public:
    static ColorSchemer *create(QQmlEngine *, QJSEngine *)
    {
        auto inst = &instance();
        QJSEngine::setObjectOwnership(inst, QJSEngine::ObjectOwnership::CppOwnership);
        return inst;
    }

    explicit ColorSchemer(QObject *parent = nullptr);

    static ColorSchemer &instance();

    [[nodiscard]] QAbstractItemModel *model() const;
    Q_INVOKABLE void apply(int idx);
    Q_INVOKABLE void apply(const QString &name);
    Q_INVOKABLE [[nodiscard]] int indexForScheme(const QString &name) const;
    Q_INVOKABLE [[nodiscard]] QString nameForIndex(int index) const;

private:
    KColorSchemeManager *c;
};
