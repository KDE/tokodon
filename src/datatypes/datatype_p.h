// SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "datatype.h"

#include <QSharedData>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>

#define TOKODON_MAKE_GADGET(Class) \
Class::Class() : d(new Class ## Private) {} \
Class::Class(const Class&) = default; \
Class::Class(Class&&) noexcept = default; \
Class::~Class() = default; \
Class& Class::operator=(const Class&) = default; \
Class& Class::operator=(Class&&) noexcept = default; \
Class::operator QVariant() const { return QVariant::fromValue(*this); } \
QList<Class> Class::fromJson(const QJsonArray &array) \
{\
    QList<Class> types; \
    std::ranges::transform(array, std::back_inserter(types), [](const QJsonValue &value) -> Class { \
        return Class::fromJson(value.toObject()); \
    });\
    return types; \
}\

// TODO this could be improved by checking for equality before detaching
// but this requires similar equality workarounds than KItinerary uses
#define TOKODON_MAKE_PROPERTY(Class, Type, Getter, Setter) \
Type Class::Getter() const { return d->Getter; } \
void Class::Setter(Tokdon::Internal::parameter_type<Type>::type value) \
{ \
    d.detach(); \
    d->Getter = value; \
}

#define TOKODON_MAKE_PROPERTY_READONLY(Class, Type, Getter) \
Type Class::Getter() const { return d->Getter; }
