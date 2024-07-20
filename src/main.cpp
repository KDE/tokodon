// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QQuickStyle>
#include <QQuickWindow>

#ifdef HAVE_WEBVIEW
#include <QtWebView>
#endif

#ifdef Q_OS_ANDROID
#include "utils/androidutils.h"
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#ifdef HAVE_KDBUSADDONS
#include <KDBusService>
#include <KWindowSystem>
#endif
#include <KLocalizedString>

#ifdef HAVE_KUNIFIEDPUSH
#include <kunifiedpush/connector.h>
#endif

#include "tokodon-version.h"

#include "account/accountmanager.h"
#include "accountconfig.h"
#include "admin/emailinfo.h"
#include "admin/ipinfo.h"
#include "config.h"
#include "network/networkaccessmanagerfactory.h"
#include "network/networkcontroller.h"
#include "tokodon_debug.h"
#include "utils/blurhashimageprovider.h"
#include "utils/colorschemer.h"

#ifdef Q_OS_WINDOWS
#include <Windows.h>
#endif

#ifdef TEST_MODE
#include "autotests/helperreply.h"
#include "autotests/mockaccount.h"
#endif

#if __has_include("KCrash")
#include <KCrash>
#endif

using namespace Qt::Literals::StringLiterals;

void setupUnifiedPush()
{
#ifdef HAVE_KUNIFIEDPUSH
    auto connector = new KUnifiedPush::Connector(QStringLiteral("org.kde.tokodon"));
    QObject::connect(connector, &KUnifiedPush::Connector::endpointChanged, [=](const auto &endpoint) {
        NetworkController::instance().endpoint = endpoint;
    });

    NetworkController::instance().endpoint = connector->endpoint();

    connector->registerClient(i18n("Receiving push notifications"));
#endif
}

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char *argv[])
{
    QNetworkProxyFactory::setUseSystemConfiguration(true);

#ifdef HAVE_WEBVIEW
    QtWebView::initialize();
#endif

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    app.connect(&app, &QGuiApplication::applicationStateChanged, [](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive) {
            AndroidUtils::instance().checkPendingIntents();
        }
    });
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
#else
    QApplication app(argc, argv);
    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
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
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("tokodon"));

    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));

    KAboutData about(QStringLiteral("tokodon"),
                     i18n("Tokodon"),
                     QStringLiteral(TOKODON_VERSION_STRING),
                     i18n("Mastodon client"),
                     KAboutLicense::GPL_V3,
                     i18n("© 2021-2024 Carl Schwan, 2021-2024 KDE Community"));
    about.addAuthor(i18n("Carl Schwan"),
                    i18n("Maintainer"),
                    QStringLiteral("carl@carlschwan.eu"),
                    QStringLiteral("https://carlschwan.eu"),
                    QUrl(QStringLiteral("https://carlschwan.eu/avatar.png")));
    about.addAuthor(i18n("Joshua Goins"),
                    i18n("Maintainer"),
                    QStringLiteral("josh@redstrate.com"),
                    QStringLiteral("https://redstrate.com/"),
                    QUrl(QStringLiteral("https://redstrate.com/rss-image.png")));
    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    about.setOrganizationDomain("kde.org");
    about.setBugAddress("https://bugs.kde.org/enter_bug.cgi?product=Tokodon&component=general");

    KAboutData::setApplicationData(about);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.tokodon")));

#if __has_include("KCrash")
    KCrash::initialize();
#endif

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("Client for decentralized social networks like Mastodon"));
    parser.addPositionalArgument(QStringLiteral("urls"), i18n("Supports https and web+ap url scheme"));

    QCommandLineOption shareOption(QStringLiteral("share"), i18n("Share a line of text in the standalone composer."), i18n("The text to share."));
    shareOption.setFlags(QCommandLineOption::Flag::HiddenFromHelp);
    parser.addOption(shareOption);

    QCommandLineOption notifyOption(QStringLiteral("dbus-activated"), i18n("Internal usage only."));
    notifyOption.setFlags(QCommandLineOption::Flag::HiddenFromHelp);
    parser.addOption(notifyOption);

    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    auto &colorSchemer = ColorSchemer::instance();
    auto config = Config::self();
    if (!config->colorScheme().isEmpty()) {
        colorSchemer.apply(config->colorScheme());
    }

    qmlRegisterSingletonInstance("org.kde.tokodon.private", 1, 0, "Config", config);
    qmlRegisterUncreatableType<AccountConfig>("org.kde.tokodon.private", 1, 0, "AccountConfig", QStringLiteral("Use via Account.config"));
    qmlRegisterUncreatableMetaObject(Notification::staticMetaObject, "org.kde.tokodon", 1, 0, "Notification", QStringLiteral("Only for enums"));
    qmlRegisterUncreatableMetaObject(IpInfo::staticMetaObject, "org.kde.tokodon", 1, 0, "IpInfo", QStringLiteral("Only for enums"));
    qmlRegisterUncreatableMetaObject(EmailInfo::staticMetaObject, "org.kde.tokodon", 1, 0, "EmailInfo", QStringLiteral("Only for enums"));

    QQmlApplicationEngine engine;

#ifdef HAVE_KUNIFIEDPUSH
    if (parser.isSet(notifyOption)) {
        qInfo(TOKODON_LOG) << "Beginning to check for notifications...";

#ifdef HAVE_KDBUSADDONS
        // We want to be replaceable by the main client
        KDBusService service(KDBusService::Replace);
#endif

        setupUnifiedPush();

        // create the lazy instance
        AccountManager::instance().loadFromSettings();

        QObject::connect(&AccountManager::instance(), &AccountManager::accountsReady, [] {
            qInfo(TOKODON_LOG) << "Accounts have finished loading. Checking notification queue...";
            // queue notification
            AccountManager::instance().queueNotifications();
        });

        QObject::connect(AccountManager::instance().notificationHandler(), &NotificationHandler::lastNotificationClosed, qApp, &QCoreApplication::quit);
        QObject::connect(&AccountManager::instance(), &AccountManager::finishedNotificationQueue, qApp, &QCoreApplication::quit);

        return QCoreApplication::exec();
    }
#endif

#ifdef HAVE_KDBUSADDONS
    KDBusService service(KDBusService::Unique);
#endif

    setupUnifiedPush();

#ifdef HAVE_KDBUSADDONS
    QObject::connect(&service, &KDBusService::activateRequested, &engine, [&engine](const QStringList &arguments, const QString & /*workingDirectory*/) {
        const auto rootObjects = engine.rootObjects();
        for (auto obj : rootObjects) {
            if (auto view = qobject_cast<QQuickWindow *>(obj)) {
                KWindowSystem::updateStartupId(view);
                KWindowSystem::activateWindow(view);

                if (arguments.isEmpty()) {
                    return;
                }

                auto args = arguments;
                args.removeFirst();

                if (!args.empty()) {
                    if (args[0].startsWith("web+ap"_L1)) {
                        NetworkController::instance().openWebApLink(args[0]);
                    } else if (args[0].startsWith("tokodon"_L1)) {
                        NetworkController::instance().setAuthCode(QUrl(args[0]));
                    } else if (args[0] == "--share"_L1) {
                        NetworkController::instance().startComposing(args[1]);
                    } else {
                        NetworkController::instance().openWebApLink(args[0]);
                    }
                }

                return;
            }
        }
    });
#endif
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QCoreApplication::quit);

    NetworkAccessManagerFactory namFactory;
    engine.setNetworkAccessManagerFactory(&namFactory);

    engine.addImageProvider(QLatin1String("blurhash"), new BlurHashImageProvider);

#ifdef TEST_MODE
    AccountManager::instance().setTestMode(true);

    auto account = new MockAccount();
    AccountManager::instance().addAccount(account, true);
    AccountManager::instance().selectAccount(account);

    QUrl url = account->apiUrl(QStringLiteral("/api/v2/search"));
    url.setQuery(QUrlQuery{{QStringLiteral("q"), QStringLiteral("myquery")}, {QStringLiteral("resolve"), QStringLiteral("true")}});
    account->registerGet(url, new TestReply(QStringLiteral("search-result.json"), account));

    account->registerGet(account->apiUrl(QStringLiteral("/api/v1/timelines/home")), new TestReply(QStringLiteral("statuses.json"), account));
#else
    AccountManager::instance().migrateSettings();
    AccountManager::instance().loadFromSettings();
#endif

    // If this isn't initialized previous on Android, it crashes??
    // TODO: investigate why does this happen
#ifdef Q_OS_ANDROID
    Q_UNUSED(NetworkController::instance());
#endif

    if (parser.isSet(shareOption)) {
        engine.loadFromModule("org.kde.tokodon", "StandaloneComposer");

        NetworkController::instance().startComposing(parser.value(shareOption));
    } else {
        engine.loadFromModule("org.kde.tokodon", "Main");

        if (parser.positionalArguments().length() > 0) {
            NetworkController::instance().openWebApLink(parser.positionalArguments()[0]);
        }
    }

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QCoreApplication::exec();
}
