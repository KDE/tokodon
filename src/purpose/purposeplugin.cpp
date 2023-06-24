/*
    SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <QJsonArray>

#include <KPluginFactory>

#include <KIO/CommandLauncherJob>
#include <Purpose/PluginBase>

class TokodonJob : public Purpose::Job
{
    Q_OBJECT
public:
    TokodonJob(QObject *parent)
        : Purpose::Job(parent)
    {
    }

    QStringList arrayToList(const QJsonArray &array)
    {
        QStringList ret;
        for (const QJsonValue &val : array) {
            ret += val.toString();
        }
        return ret;
    }

    void start() override
    {
        const QJsonArray urlsJson = data().value(QStringLiteral("urls")).toArray();
        const QString title = data().value(QStringLiteral("title")).toString();
        const QString message = QString("%1 - %2").arg(title, arrayToList(urlsJson).join(QLatin1Char(' ')));

        auto *job = new KIO::CommandLauncherJob(QStringLiteral("tokodon"), {"--share", message});
        connect(job, &KJob::finished, this, &TokodonJob::emitResult);
        job->start();
    }
};

class Q_DECL_EXPORT PurposePlugin : public Purpose::PluginBase
{
    Q_OBJECT
public:
    PurposePlugin(QObject *p, const QVariantList &)
        : Purpose::PluginBase(p)
    {
    }

    Purpose::Job *createJob() const override
    {
        return new TokodonJob(nullptr);
    }
};

K_PLUGIN_CLASS_WITH_JSON(PurposePlugin, "purposeplugin.json")

#include "purposeplugin.moc"