// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QQmlEngine>

class SetupStep;

/**
 * @brief The C++ backing of the initial setup flow for Tokodon.
 * @note This is not related to the login flow in any way.
 */
class InitialSetupFlow : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit InitialSetupFlow(QObject *parent = nullptr);
    ~InitialSetupFlow() override;

    /**
     * @return True if any setup is needed.
     */
    Q_INVOKABLE [[nodiscard]] bool isSetupNeeded() const;

    /**
     * @return The name of the next step.
     */
    Q_INVOKABLE QString getNextStep();

private:
    QList<SetupStep *> m_steps;
    int m_currentStep = 0;
};
