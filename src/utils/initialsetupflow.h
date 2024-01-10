// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QtQml>

class SetupStep;

class InitialSetupFlow : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit InitialSetupFlow(QObject *parent = nullptr);
    ~InitialSetupFlow() override;

    Q_INVOKABLE bool isSetupNeeded() const;
    Q_INVOKABLE QString getNextStep();

private:
    QList<SetupStep *> m_steps;
    int m_currentStep = 0;
};
