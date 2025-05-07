// SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QMetaType>
#include <QSharedDataPointer>
#include <qqmlintegration.h>

#include <type_traits>

class QVariant;

namespace Tokodon {
namespace Internal {
template <typename T>
struct parameter_type
{
    using type = typename std::conditional<std::is_fundamental<T>::value || std::is_enum<T>::value, T, const T&>::type;
};
}
}

#define TOKODON_GADGET(Class, ValueType) \
    Q_GADGET \
    QML_VALUE_TYPE(ValueType) \
public: \
    Class(); \
    Class(Class &&) noexcept; \
    Class(const Class&); \
    ~Class(); \
    Class& operator=(Class&&) noexcept; \
    Class& operator=(const Class&); \
    operator QVariant () const; \
    static QList<Class> fromJson(const QJsonArray &array); \
    static Class fromJson(const QJsonObject &object); \
private: \
    friend class Class ## Private; \
    QExplicitlySharedDataPointer<Class ## Private> d;

#define TOKODON_PROPERTY(Type, Getter, Setter) \
public: \
    Q_PROPERTY(Type Getter READ Getter WRITE Setter) \
    [[nodiscard]] Type Getter() const; \
    void Setter(Tokodon::Internal::parameter_type<Type>::type value); \

#define TOKODON_PROPERTY_READONLY(Type, Getter) \
public: \
    Q_PROPERTY(Type Getter READ Getter CONSTANT) \
    [[nodiscard]] Type Getter() const;

