// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QCommandLineParser>
#include <QFontDatabase>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <clocale>

#ifdef Q_OS_ANDROID
#include "utils/androidutils.h"
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
#include "account/socialgraphmodel.h"
#include "admin/accounttoolmodel.h"
#include "admin/emailblocktoolmodel.h"
#include "admin/federationtoolmodel.h"
#include "admin/ipinfo.h"
#include "admin/iprulestoolmodel.h"
#include "config.h"
#include "conversation/conversationmodel.h"
#include "editor/attachmenteditormodel.h"
#include "editor/languagemodel.h"
#include "editor/polltimemodel.h"
#include "editor/posteditorbackend.h"
#include "network/networkaccessmanagerfactory.h"
#include "network/networkcontroller.h"
#include "network/networkrequestprogress.h"
#include "notification/notificationgroupingmodel.h"
#include "notification/notificationmodel.h"
#include "search/searchmodel.h"
#include "timeline/accountmodel.h"
#include "timeline/maintimelinemodel.h"
#include "timeline/poll.h"
#include "timeline/post.h"
#include "timeline/tagsmodel.h"
#include "timeline/tagstimelinemodel.h"
#include "timeline/threadmodel.h"
#include "utils/blurhashimageprovider.h"
#include "utils/clipboard.h"
#include "utils/colorschemer.h"
#include "utils/emojimodel.h"
#include "utils/filehelper.h"
#include "utils/mpvplayer.h"
#include "utils/navigation.h"

#ifdef Q_OS_WINDOWS
#include <Windows.h>
#endif

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char *argv[])
{
    QNetworkProxyFactory::setUseSystemConfiguration(true);

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    app.connect(&app, &QGuiApplication::applicationStateChanged, [](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive) {
            AndroidUtils::instance().checkPendingIntents();
        }
    });
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
    QIcon::setThemeName("tokodon");
#else
    QApplication app(argc, argv);
    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    // Breeze theme needs to be the "primary" theme or else icons do not get recolored
    // Non-Breeze themes (like Adwaita) do not load our icons properly unless tokodon is the main icon theme.
    if (QIcon::themeName() == "breeze") {
        QIcon::setFallbackThemeName("tokodon");
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
    about.addAuthor(i18n("Carl Schwan"),
                    i18n("Maintainer"),
                    QStringLiteral("carl@carlschwan.eu"),
                    QStringLiteral("https://carlschwan.eu"),
                    QStringLiteral("https://carlschwan.eu/avatar.png"));
    about.addAuthor(i18n("Joshua Goins"),
                    i18n("Maintainer"),
                    QStringLiteral("josh@redstrate.com"),
                    QStringLiteral("https://redstrate.com/"),
                    QStringLiteral("https://redstrate.com/rss-image.png"));
    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    about.setOrganizationDomain("kde.org");
    about.setBugAddress("https://bugs.kde.org/describecomponents.cgi?product=tokodon");

    KAboutData::setApplicationData(about);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.tokodon")));

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("Client for the decentralized social network: mastodon"));
    parser.addPositionalArgument(QStringLiteral("urls"), i18n("Supports web+ap: url scheme"));

    QCommandLineOption shareOption("share", i18n("Share a line of text in the standalone composer."), i18n("The text to share."));
    parser.addOption(shareOption);

    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    auto config = Config::self();

    ColorSchemer colorScheme;
    qmlRegisterSingletonInstance<ColorSchemer>("org.kde.kmasto", 1, 0, "ColorSchemer", &colorScheme);
    if (!config->colorScheme().isEmpty()) {
        colorScheme.apply(config->colorScheme());
    }

    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
    setlocale(LC_NUMERIC, "C");

    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "Config", config);
    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "Controller", &NetworkController::instance());
    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "AccountManager", &AccountManager::instance());
    qmlRegisterType<MainTimelineModel>("org.kde.kmasto", 1, 0, "MainTimelineModel");
    qmlRegisterType<SearchModel>("org.kde.kmasto", 1, 0, "SearchModel");
    qmlRegisterType<SocialGraphModel>("org.kde.kmasto", 1, 0, "SocialGraphModel");
    qmlRegisterType<AccountsToolModel>("org.kde.kmasto", 1, 0, "AccountsToolModel");
    qmlRegisterType<FederationToolModel>("org.kde.kmasto", 1, 0, "FederationToolModel");
    qmlRegisterType<IpRulesToolModel>("org.kde.kmasto", 1, 0, "IpRulesToolModel");
    qmlRegisterType<EmailBlockToolModel>("org.kde.kmasto", 1, 0, "EmailBlockToolModel");
    qmlRegisterType<TagsModel>("org.kde.kmasto", 1, 0, "TagsModel");
    qmlRegisterType<ThreadModel>("org.kde.kmasto", 1, 0, "ThreadModel");
    qmlRegisterType<ConversationModel>("org.kde.kmasto", 1, 0, "ConversationModel");
    qmlRegisterType<TagsTimelineModel>("org.kde.kmasto", 1, 0, "TagsTimelineModel");
    qmlRegisterType<AccountModel>("org.kde.kmasto", 1, 0, "AccountModel");
    qmlRegisterType<ProfileEditorBackend>("org.kde.kmasto", 1, 0, "ProfileEditorBackend");
    qmlRegisterType<NetworkRequestProgress>("org.kde.kmasto", 1, 0, "NetworkRequestProgress");
    qmlRegisterType<PostEditorBackend>("org.kde.kmasto", 1, 0, "PostEditorBackend");
    qmlRegisterType<NotificationModel>("org.kde.kmasto", 1, 0, "NotificationModel");
    qmlRegisterType<PollTimeModel>("org.kde.kmasto", 1, 0, "PollTimeModel");
    qmlRegisterType<NotificationGroupingModel>("org.kde.kmasto", 1, 0, "NotificationGroupingModel");
    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "Clipboard", new Clipboard);
    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "FileHelper", new FileHelper);
    qmlRegisterSingletonType("org.kde.kmasto", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });
    qRegisterMetaType<Account *>("Account*");
    qRegisterMetaType<AbstractAccount *>("AbstractAccount*");
    qRegisterMetaType<Identity *>("Identity*");
    qRegisterMetaType<AdminAccountInfo *>("AdminAccountInfo*");
    qRegisterMetaType<AttachmentEditorModel *>("AttachmentEditorModel*");
    qRegisterMetaType<Post *>("Post*");
    qRegisterMetaType<Tag *>("Tag*");
    qRegisterMetaType<Poll *>("Poll*");
    qRegisterMetaType<Card *>("Card*");
    qRegisterMetaType<Application *>("Application*");
    qRegisterMetaType<QNetworkReply *>("QNetworkReply*");
    qRegisterMetaType<Relationship *>("Relationship*");
    qmlRegisterUncreatableType<Tag>("org.kde.kmasto", 1, 0, "Tag", "ENUM");
    qmlRegisterUncreatableType<Post>("org.kde.kmasto", 1, 0, "Post", "ENUM");
    qmlRegisterUncreatableType<AdminAccountInfo>("org.kde.kmasto", 1, 0, "AdminAccountInfo", "ENUM");
    qmlRegisterUncreatableType<Attachment>("org.kde.kmasto", 1, 0, "Attachment", "ENUM");
    qmlRegisterUncreatableType<Notification>("org.kde.kmasto", 1, 0, "Notification", "ENUM");
    qmlRegisterUncreatableType<IpInfo>("org.kde.kmasto", 1, 0, "IpInfo", "ENUM");
    qmlRegisterUncreatableType<Poll>("org.kde.kmasto", 1, 0, "Poll", "ENUM");
    qmlRegisterSingletonType(QUrl("qrc:/content/ui/Navigation.qml"), "org.kde.kmasto", 1, 0, "Navigation");
    qmlRegisterType<LanguageModel>("org.kde.kmasto", 1, 0, "LanguageModel");
    qmlRegisterType<MpvPlayer>("org.kde.kmasto", 1, 0, "MpvPlayer");
    qmlRegisterSingletonInstance("org.kde.kmasto", 1, 0, "EmojiModel", &EmojiModel::instance());
    qmlRegisterSingletonType<Navigation>("org.kde.kmasto", 1, 0, "Navigation", [](QQmlEngine *engine, QJSEngine *) {
        return new Navigation;
    });

    QQmlApplicationEngine engine;
#ifdef HAVE_KDBUSADDONS
    KDBusService service(KDBusService::Unique);
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

                if (arguments.length() >= 1) {
                    if (args[0].startsWith("web+ap")) {
                        NetworkController::instance().openWebApLink(args[0]);
                    } else {
                        NetworkController::instance().setAuthCode(QUrl(args[0]));
                    }
                    if (args[0] == "--share") {
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

    // Controller::instance().setAboutData(about);

    engine.addImageProvider(QLatin1String("blurhash"), new BlurhashImageProvider);

    if (parser.isSet(shareOption)) {
        engine.load(QUrl(QStringLiteral("qrc:/content/ui/StandaloneComposer.qml")));

        NetworkController::instance().startComposing(parser.value(shareOption));
    } else {
        engine.load(QUrl(QStringLiteral("qrc:/content/ui/main.qml")));

        if (parser.positionalArguments().length() > 0) {
            NetworkController::instance().openWebApLink(parser.positionalArguments()[0]);
        }
    }

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
#ifdef HAVE_KDBUSADDONS
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
