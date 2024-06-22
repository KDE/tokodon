// SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "utils/initialsetupflow.h"

#include "config.h"

#include <qt6keychain/keychain.h>

#include <KNotificationPermission>

class SetupStep : public QObject
{
public:
    explicit SetupStep(QObject *parent)
        : QObject(parent)
    {
    }

    virtual bool isSetupNeeded() = 0;
    virtual QString moduleName() = 0;
};

class SetupPasswordStep final : public SetupStep
{
public:
    explicit SetupPasswordStep(QObject *parent)
        : SetupStep(parent)
    {
    }

    bool isSetupNeeded() override
    {
        // Sailfish doesn't have a secure backend yet, so always skip this setup
#if defined(SAILFISHOS) || defined(KDECI)
        return false;
#else
        return !QKeychain::isAvailable();
#endif
    }

    QString moduleName() override
    {
        return QStringLiteral("SetupPassword");
    }
};

class SetupNotificationsStep final : public SetupStep
{
public:
    explicit SetupNotificationsStep(QObject *parent)
        : SetupStep(parent)
    {
    }

    bool isSetupNeeded() override
    {
#ifdef Q_OS_ANDROID
        // Don't show the notification permission prompt if the user asked not to
        auto config = Config::self();
        return !config->promptedNotificationPermission();
#else
        return false;
#endif
    }

    QString moduleName() override
    {
        return QStringLiteral("SetupNotifications");
    }
};

InitialSetupFlow::InitialSetupFlow(QObject *parent)
    : QObject(parent)
{
    m_steps = {new SetupPasswordStep(this), new SetupNotificationsStep(this)};
}

InitialSetupFlow::~InitialSetupFlow() = default;

bool InitialSetupFlow::isSetupNeeded() const
{
    return std::any_of(m_steps.cbegin(), m_steps.cend(), [](SetupStep *step) {
        return step->isSetupNeeded();
    });
}

QString InitialSetupFlow::getNextStep()
{
    while (true) {
        if (m_currentStep > m_steps.size()) {
            break;
        }

        if (m_steps[m_currentStep]->isSetupNeeded()) {
            return m_steps[m_currentStep]->moduleName();
        } else {
            m_currentStep++;
        }
    }

    return QString();
}

#include "moc_initialsetupflow.cpp"
