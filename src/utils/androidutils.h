// SPDX-FileCopyrightText: 2023 Joshua goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later OR GPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

#include <QObject>

class AndroidUtils : public QObject
{
    Q_OBJECT
public:
    static AndroidUtils &instance();

    void checkPendingIntents();

private:
    explicit AndroidUtils(QObject *parent = nullptr);
};
