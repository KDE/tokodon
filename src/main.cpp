/**
 * SPDX-FileCopyrightText: 2018-2019 Black Hat <bhat@encom.eu.org>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include <QCommandLineParser>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QApplication>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>

#include <KAboutData>
#ifdef HAVE_KDBUSADDONS
#include <KDBusService>
#endif
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KWindowConfig>
#include <KConfig>

#include "tokodon-version.h"

#include "accountmanager.h"
#include "account.h"
#include "attachmenteditormodel.h"
#include "timelinemodel.h"
#include "post.h"
#include "clipboard.h"

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QNetworkProxyFactory::setUseSystemConfiguration(true);

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("Material"));
#else
    QApplication app(argc, argv);
    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#endif

#ifdef Q_OS_WINDOWS
    QApplication::setStyle(QStringLiteral("breeze"));
#endif

    QApplication::setOrganizationName("KDE");

    KAboutData about(QStringLiteral("tokodon"), i18n("Tokodon"), QStringLiteral(TOKODON_VERSION_STRING), i18n("Mastodon client"), KAboutLicense::GPL_V3, i18n("© 2021 Carl Schwan, 2021 KDE Community"));
    about.addAuthor(i18n("Carl Schwan"), QString(), QStringLiteral("carl@carlschwan.eu"));
    about.setOrganizationDomain("kde.org");

    KAboutData::setApplicationData(about);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.tokodon")));

#ifdef HAVE_KDBUSADDONS
    KDBusService service(KDBusService::Unique);
#endif

    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "AccountManager", &AccountManager::instance());
    qmlRegisterType<TimelineModel>("org.kde.kmasto", 1, 0, "TimelineModel");
    qmlRegisterSingletonType<Clipboard>("org.kde.kmasto", 1, 0, "Clipboard", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new Clipboard;
    });
    qmlRegisterType<AttachmentEditorModel>("org.kde.kmasto", 1, 0, "AttachmentEditorModel");
    qRegisterMetaType<Account *>("Account*");
    qRegisterMetaType<Post *>("Post*");
    qRegisterMetaType<Identity *>("Identity*");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    KLocalizedString::setApplicationDomain("kmasto");
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QCoreApplication::quit);

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("Client for the decentralized social network: mastodon"));
    parser.addVersionOption();

    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    //Controller::instance().setAboutData(about);

    engine.load(QUrl(QStringLiteral("qrc:/content/ui/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

#ifdef HAVE_KDBUSADDONS
    QObject::connect(&service, &KDBusService::activateRequested, &engine, [&engine](const QStringList & /*arguments*/, const QString & /*workingDirectory*/) {
        const auto rootObjects = engine.rootObjects();
        for (auto obj : rootObjects) {
            auto view = qobject_cast<QQuickWindow *>(obj);
            if (view) {
                view->show();
                view->raise();
                return;
            }
        }
    });
    const auto rootObjects = engine.rootObjects();
    for (auto obj : rootObjects) {
        auto view = qobject_cast<QQuickWindow*>(obj);
        if (view) {
            KConfig dataResource("data", KConfig::SimpleConfig, QStandardPaths::AppDataLocation);
            KConfigGroup windowGroup(&dataResource, "Window");
            KWindowConfig::restoreWindowSize(view, windowGroup);
            KWindowConfig::restoreWindowPosition(view, windowGroup);
            break;
        }
    }
#endif
    return QApplication::exec();
}
