/**
 * SPDX-FileCopyrightText: 2018-2019 Black Hat <bhat@encom.eu.org>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include <QCommandLineParser>
#include <QFontDatabase>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>

#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#include <KAboutData>
#ifdef HAVE_KDBUSADDONS
#include <KDBusService>
#include <KWindowSystem>
#endif
#include <KConfig>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KWindowConfig>

#include "tokodon-version.h"

#include "account/account.h"
#include "account/accountmanager.h"
#include "account/profileeditor.h"
#include "config.h"
#include "conversation/conversationmodel.h"
#include "editor/attachmenteditormodel.h"
#include "editor/languagemodel.h"
#include "editor/polltimemodel.h"
#include "editor/posteditorbackend.h"
#include "timeline/linkpaginatedtimelinemodel.h"
#include "timeline/maintimelinemodel.h"
#include "network/networkaccessmanagerfactory.h"
#include "network/networkcontroller.h"
#include "network/networkrequestprogress.h"
#include "timeline/accountmodel.h"
#include "timeline/notificationmodel.h"
#include "timeline/poll.h"
#include "timeline/post.h"
#include "timeline/tagsmodel.h"
#include "timeline/threadmodel.h"
#include "search/searchmodel.h"
#include "account/socialgraphmodel.h"
#include "utils/blurhashimageprovider.h"
#include "utils/clipboard.h"
#include "utils/filehelper.h"

#ifdef HAVE_COLORSCHEME
#include "utils/colorschemer.h"
#endif

#ifdef Q_OS_WINDOWS
#include <Windows.h>
#endif

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QNetworkProxyFactory::setUseSystemConfiguration(true);

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
    QIcon::setThemeName("tokodon");
#else
    QApplication app(argc, argv);
    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    if (QIcon::themeName().contains("dark")) {
        QIcon::setThemeName("tokodon-dark");
    } else {
        QIcon::setThemeName("tokodon");
    }
#endif

#ifdef Q_OS_WINDOWS
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    QApplication::setStyle(QStringLiteral("breeze"));
    auto font = app.font();
    font.setPointSize(10);
    app.setFont(font);
#endif
    KLocalizedString::setApplicationDomain("tokodon");

    QCoreApplication::setOrganizationName("KDE");

    KAboutData about(QStringLiteral("tokodon"),
                     i18n("Tokodon"),
                     QStringLiteral(TOKODON_VERSION_STRING),
                     i18n("Mastodon client"),
                     KAboutLicense::GPL_V3,
                     i18n("© 2021-2023 Carl Schwan, 2021-2023 KDE Community"));
    about.addAuthor(i18n("Carl Schwan"), i18n("Maintainer"), QStringLiteral("carl@carlschwan.eu"));
    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    about.setOrganizationDomain("kde.org");
    about.setBugAddress("https://bugs.kde.org/describecomponents.cgi?product=tokodon");

    KAboutData::setApplicationData(about);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.tokodon")));

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("Client for the decentralized social network: mastodon"));
    parser.addPositionalArgument(QStringLiteral("urls"), i18n("Supports web+ap: url scheme"));

    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

#ifdef HAVE_KDBUSADDONS
    KDBusService service(KDBusService::Unique);
#endif
    auto config = Config::self();

#ifdef HAVE_COLORSCHEME
    ColorSchemer colorScheme;
    qmlRegisterSingletonInstance<ColorSchemer>("org.kde.kmasto", 1, 0, "ColorSchemer", &colorScheme);
    if (!config->colorScheme().isEmpty()) {
        colorScheme.apply(config->colorScheme());
    }
#endif

    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "Config", config);
    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "Controller", &NetworkController::instance());
    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "AccountManager", &AccountManager::instance());
    qmlRegisterType<MainTimelineModel>("org.kde.kmasto", 1, 0, "TimelineModel");
    qmlRegisterType<LinkPaginationTimelineModel>("org.kde.kmasto", 1, 0, "LinkPaginationTimelineModel");
    qmlRegisterType<SearchModel>("org.kde.kmasto", 1, 0, "SearchModel");
    qmlRegisterType<SocialGraphModel>("org.kde.kmasto", 1, 0, "SocialGraphModel");
    qmlRegisterType<ThreadModel>("org.kde.kmasto", 1, 0, "ThreadModel");
    qmlRegisterType<ConversationModel>("org.kde.kmasto", 1, 0, "ConversationModel");
    qmlRegisterType<TagsModel>("org.kde.kmasto", 1, 0, "TagsModel");
    qmlRegisterType<AccountModel>("org.kde.kmasto", 1, 0, "AccountModel");
    qmlRegisterType<ProfileEditorBackend>("org.kde.kmasto", 1, 0, "ProfileEditorBackend");
    qmlRegisterType<NetworkRequestProgress>("org.kde.kmasto", 1, 0, "NetworkRequestProgress");
    qmlRegisterType<PostEditorBackend>("org.kde.kmasto", 1, 0, "PostEditorBackend");
    qmlRegisterType<NotificationModel>("org.kde.kmasto", 1, 0, "NotificationModel");
    qmlRegisterType<PollTimeModel>("org.kde.kmasto", 1, 0, "PollTimeModel");
    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "Clipboard", new Clipboard);
    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "FileHelper", new FileHelper);
    qmlRegisterSingletonType("org.kde.kmasto", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });
    qRegisterMetaType<Account *>("Account*");
    qRegisterMetaType<AbstractAccount *>("AbstractAccount*");
    qRegisterMetaType<Identity *>("Identity*");
    qRegisterMetaType<AttachmentEditorModel *>("AttachmentEditorModel*");
    qRegisterMetaType<Post *>("Post*");
    qRegisterMetaType<Poll *>("Poll*");
    qRegisterMetaType<Card *>("Card*");
    qRegisterMetaType<Application *>("Application*");
    qRegisterMetaType<QNetworkReply *>("QNetworkReply*");
    qRegisterMetaType<Relationship *>("Relationship*");
    qmlRegisterUncreatableType<Post>("org.kde.kmasto", 1, 0, "Post", "ENUM");
    qmlRegisterUncreatableType<Attachment>("org.kde.kmasto", 1, 0, "Attachment", "ENUM");
    qmlRegisterUncreatableType<Notification>("org.kde.kmasto", 1, 0, "Notification", "ENUM");
    qmlRegisterSingletonType(QUrl("qrc:/content/ui/Navigation.qml"), "org.kde.kmasto", 1, 0, "Navigation");
    qmlRegisterType<LanguageModel>("org.kde.kmasto", 1, 0, "LanguageModel");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QCoreApplication::quit);

    NetworkAccessManagerFactory namFactory;
    engine.setNetworkAccessManagerFactory(&namFactory);

    // Controller::instance().setAboutData(about);

    engine.addImageProvider(QLatin1String("blurhash"), new BlurhashImageProvider);

    engine.load(QUrl(QStringLiteral("qrc:/content/ui/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    if (parser.positionalArguments().length() > 0) {
        NetworkController::instance().openWebApLink(parser.positionalArguments()[0]);
    }

#ifdef HAVE_KDBUSADDONS
    QObject::connect(&service, &KDBusService::activateRequested, &engine, [&engine](const QStringList &arguments, const QString & /*workingDirectory*/) {
        const auto rootObjects = engine.rootObjects();
        for (auto obj : rootObjects) {
            auto view = qobject_cast<QQuickWindow *>(obj);
            if (view) {
                KWindowSystem::updateStartupId(view);
                KWindowSystem::activateWindow(view);

                if (arguments.isEmpty()) {
                    return;
                }

                auto args = arguments;
                args.removeFirst();

                if (arguments.length() >= 1) {
                    NetworkController::instance().openWebApLink(args[0]);
                }

                return;
            }
        }
    });
    const auto rootObjects = engine.rootObjects();
    for (auto obj : rootObjects) {
        auto view = qobject_cast<QQuickWindow *>(obj);
        if (view) {
            KConfig dataResource("data", KConfig::SimpleConfig, QStandardPaths::AppDataLocation);
            KConfigGroup windowGroup(&dataResource, "Window");
            KWindowConfig::restoreWindowSize(view, windowGroup);
            KWindowConfig::restoreWindowPosition(view, windowGroup);
            break;
        }
    }
#endif
    return QCoreApplication::exec();
}
