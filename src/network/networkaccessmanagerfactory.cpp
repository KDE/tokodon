// SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "networkaccessmanagerfactory.h"

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QStandardPaths>
#include <QThread>

QNetworkAccessManager *NetworkAccessManagerFactory::create(QObject *parent)
{
    auto nam = new QNetworkAccessManager(parent);

    nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);

    nam->enableStrictTransportSecurityStore(true, QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/hsts/"));
    nam->setStrictTransportSecurityEnabled(true);

    auto namDiskCache = new QNetworkDiskCache(nam);
    namDiskCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/nam/"));
    nam->setCache(namDiskCache);

    return nam;
}
