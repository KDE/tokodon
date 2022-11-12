// SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QQmlNetworkAccessManagerFactory>

class NetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory
{
public:
    QNetworkAccessManager *create(QObject *parent) override;
};
