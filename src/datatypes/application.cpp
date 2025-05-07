// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL


#include "application.h"
#include "datatype_p.h"

using namespace Qt::StringLiterals;

class ApplicationPrivate : public QSharedData
{
public:
    QString name;
    QString website;
}

TOKODON_MAKE_GADGET(Application)
TOKODON_MAKE_PROPERTY_READONLY(Application, QString, name)
TOKODON_MAKE_PROPERTY_READONLY(Application, QString, website)

Application Application::fromJson(const QJsonObject &object)
{
    Application application;
    application.d->name = object["name"_L1].toString();
    application.d->website = object["website"_L1].toString();
    return application;
}
