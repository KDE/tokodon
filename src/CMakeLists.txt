# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_library(tokodon_static STATIC)

ecm_add_qml_module(tokodon_static GENERATE_PLUGIN_SOURCE
        OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/tokodon # Shuts up an annoying upstream Qt warning
        URI org.kde.tokodon)

ecm_qt_declare_logging_category(tokodon_static
    HEADER tokodon_debug.h
    IDENTIFIER TOKODON_LOG
    CATEGORY_NAME org.kde.tokodon
    DESCRIPTION "tokodon"
    DEFAULT_SEVERITY Warning
    EXPORT TOKODON
)

ecm_qt_declare_logging_category(tokodon_static
    HEADER tokodon_http_debug.h
    IDENTIFIER TOKODON_HTTP
    CATEGORY_NAME org.kde.tokodon.http
    DESCRIPTION "Tokodon request"
    EXPORT TOKODON
)

target_sources(tokodon_static
    PRIVATE

    # Shell
    tokodonapplication.cpp
    tokodonapplication.h

    # Account
    account/abstractaccount.cpp
    account/abstractaccount.h
    account/accountmanager.cpp
    account/accountmanager.h
    account/announcementmodel.cpp
    account/announcementmodel.h
    account/featuredtagsmodel.cpp
    account/featuredtagsmodel.h
    account/account.cpp
    account/account.h
    account/preferences.cpp
    account/preferences.h
    account/identity.cpp
    account/identity.h
    account/listsmodel.cpp
    account/listsmodel.h
    account/scheduledstatusesmodel.cpp
    account/scheduledstatusesmodel.h
    account/socialgraphmodel.cpp
    account/socialgraphmodel.h
    account/suggestionsmodel.cpp
    account/suggestionsmodel.h
    account/relationship.cpp
    account/rulesmodel.cpp
    account/rulesmodel.h
    account/profileeditor.cpp
    account/publicserversmodel.cpp
    account/publicserversmodel.h
    account/notificationhandler.cpp
    account/notificationhandler.h
    account/notificationfilteringpolicy.cpp
    account/notificationfilteringpolicy.h
    account/annualreport.cpp
    account/annualreport.h
    account/favoritelistsmodel.cpp
    account/favoritelistsmodel.h
    account/filtersmodel.cpp
    account/filtersmodel.h
    account/blockeddomainmodel.cpp
    account/blockeddomainmodel.h
    account/serverinformation.cpp
    account/serverinformation.h

    # Editor
    editor/posteditorbackend.cpp
    editor/posteditorbackend.h
    editor/attachmenteditormodel.cpp
    editor/attachmenteditormodel.h
    editor/polltimemodel.cpp
    editor/polltimemodel.h
    editor/languagemodel.cpp
    editor/languagemodel.h
    editor/listeditorbackend.cpp
    editor/listeditorbackend.h
    editor/polleditorbackend.cpp
    editor/polleditorbackend.h
    editor/reporteditorbackend.cpp
    editor/reporteditorbackend.h
    editor/filtereditorbackend.cpp
    editor/filtereditorbackend.h

    # Conversation view
    conversation/conversationmodel.cpp
    conversation/conversationmodel.h

    # Data types
    datatypes/application.cpp
    datatypes/application.h
    datatypes/attachment.cpp
    datatypes/attachment.h
    datatypes/card.cpp
    datatypes/card.h
    datatypes/poll.cpp
    datatypes/poll.h
    datatypes/post.cpp
    datatypes/post.h
    datatypes/tag.h
    datatypes/tag.cpp

    # Timeline
    timeline/notification.cpp
    timeline/notification.h
    timeline/accountmodel.cpp
    timeline/accountmodel.h
    timeline/threadmodel.cpp
    timeline/threadmodel.h
    timeline/timelinemodel.cpp
    timeline/timelinemodel.h
    timeline/tagstimelinemodel.cpp
    timeline/tagstimelinemodel.h
    timeline/maintimelinemodel.cpp
    timeline/maintimelinemodel.h
    timeline/abstracttimelinemodel.cpp
    timeline/abstracttimelinemodel.h
    timeline/accountmediatimelinemodel.cpp
    timeline/accountmediatimelinemodel.h
    timeline/tagsmodel.h
    timeline/tagsmodel.cpp
    timeline/abstractlistmodel.cpp
    timeline/abstractlistmodel.h
    timeline/trendingnewsmodel.cpp
    timeline/trendingnewsmodel.h

    # Search
    search/searchmodel.cpp
    search/searchmodel.h

    # Misc utils
    utils/blurhash.cpp
    utils/blurhash.h
    utils/blurhashimageprovider.cpp
    utils/blurhashimageprovider.h
    utils/filehelper.cpp
    utils/filehelper.h
    utils/filetransferjob.cpp
    utils/filetransferjob.h
    utils/initialsetupflow.cpp
    utils/initialsetupflow.h
    utils/limitermodel.cpp
    utils/limitermodel.h
    utils/navigation.cpp
    utils/navigation.h
    utils/emojimodel.cpp
    utils/emojimodel.h
    utils/emojis.h
    utils/emojitones.cpp
    utils/emojitones.h
    utils/emojitones_data.h
    utils/messagefiltercontainer.cpp
    utils/messagefiltercontainer.h
    utils/texthandler.cpp
    utils/texthandler.h
    utils/colorschemer.cpp
    utils/colorschemer.h
    utils/customemoji.cpp
    utils/customemoji.h

    # Network related classes
    network/networkrequestprogress.cpp
    network/networkrequestprogress.h
    network/networkaccessmanagerfactory.cpp
    network/networkaccessmanagerfactory.h
    network/networkcontroller.cpp
    network/networkcontroller.h

    # Admin
    admin/accounttoolmodel.cpp
    admin/accounttoolmodel.h
    admin/adminaccountinfo.cpp
    admin/adminaccountinfo.h
    admin/federationtoolmodel.cpp
    admin/federationtoolmodel.h
    admin/federationinfo.h
    admin/federationinfo.cpp
    admin/ipinfo.cpp
    admin/ipinfo.h
    admin/iprulestoolmodel.cpp
    admin/iprulestoolmodel.h
    admin/emailblocktoolmodel.cpp
    admin/emailblocktoolmodel.h
    admin/emailinfo.cpp
    admin/emailinfo.h
    admin/reporttoolmodel.cpp
    admin/reporttoolmodel.h
    admin/reportinfo.h
    admin/reportinfo.cpp

    # Notification models
    notification/notificationgroupingmodel.cpp
    notification/notificationgroupingmodel.h
    notification/notificationmodel.cpp
    notification/notificationmodel.h
)

qt_target_qml_sources(tokodon_static
    QML_FILES
        # Components
        qml/Components/Emoji/EmojiDelegate.qml
        qml/Components/Emoji/EmojiGrid.qml
        qml/Components/Emoji/EmojiPicker.qml
        qml/Components/Emoji/EmojiTonesPicker.qml
        qml/Components/ClickableIdentityInfo.qml
        qml/Components/FocusedImage.qml
        qml/Components/InlineIdentityInfo.qml
        qml/Components/PopupShadow.qml
        qml/Components/ProfileHeader.qml
        qml/Components/RoundedEffect.qml
        qml/Components/Sidebar.qml
        qml/Components/TimelineView.qml
        qml/Components/UserCard.qml
        qml/Components/QrCodeMaximizeComponent.qml
        qml/Components/BoostConfirmationDialog.qml
        qml/Components/VideoPlayer.qml
        qml/Components/ServerInformationCard.qml

        # Initial Setup
        qml/InitialSetup/SetupNotifications.qml
        qml/InitialSetup/SetupPassword.qml
        qml/InitialSetup/SetupWelcome.qml

        # Login Flow
        qml/LoginFlow/BaseAuthorization.qml
        qml/LoginFlow/BrowserAuthorization.qml
        qml/LoginFlow/CodeAuthorization.qml
        qml/LoginFlow/LoginIssuePage.qml
        qml/LoginFlow/RegistrationPage.qml
        qml/LoginFlow/RulesPage.qml
        qml/LoginFlow/ServersPage.qml
        qml/LoginFlow/WelcomePage.qml

        # Moderation Tools
        qml/ModerationTools/AccountToolPage.qml
        qml/ModerationTools/EmailBlockToolPage.qml
        qml/ModerationTools/FederationToolPage.qml
        qml/ModerationTools/IpRulePage.qml
        qml/ModerationTools/MainAccountToolPage.qml
        qml/ModerationTools/MainFederationToolPage.qml
        qml/ModerationTools/MainIpRulePage.qml
        qml/ModerationTools/MainReportToolPage.qml
        qml/ModerationTools/ModerationToolsView.qml
        qml/ModerationTools/ReportToolPage.qml

        # Notifications
        qml/Notifications/AccountWarningDelegate.qml
        qml/Notifications/GroupInteractionLabel.qml
        qml/Notifications/MinimalPostDelegate.qml
        qml/Notifications/RelationshipSeveranceDelegate.qml
        qml/Notifications/ReportDelegate.qml
        qml/Notifications/UserInteractionLabel.qml
        qml/Notifications/AnnualReportDelegate.qml
        qml/Notifications/FollowDelegate.qml
        qml/Notifications/UnknownNotificationDelegate.qml

        # Settings
        qml/Settings/AccountPage.qml
        qml/Settings/AccountsPage.qml
        qml/Settings/AppearancePage.qml
        qml/Settings/EditProfilePage.qml
        qml/Settings/ErrorLogPage.qml
        qml/Settings/NetworkProxyPage.qml
        qml/Settings/NotificationsPage.qml
        qml/Settings/ProfileEditor.qml
        qml/Settings/SafetyPage.qml
        qml/Settings/TokodonConfigurationView.qml
        qml/Settings/FiltersPage.qml
        qml/Settings/EditFilterPage.qml
        qml/Settings/BlockedDomainsPage.qml

        # Composer
        qml/StatusComposer/EditorAttachmentGrid.qml
        qml/StatusComposer/AttachmentInfoDialog.qml
        qml/StatusComposer/ComposerPoll.qml
        qml/StatusComposer/EmojiDialog.qml
        qml/StatusComposer/StatusComposer.qml
        qml/StatusComposer/StatusPreview.qml
        qml/StatusComposer/ScheduledPostsPage.qml

        # Delegate
        qml/PostDelegate/AttachmentGrid.qml
        qml/PostDelegate/AttachmentMenu.qml
        qml/PostDelegate/InformationBar.qml
        qml/PostDelegate/InteractionButton.qml
        qml/PostDelegate/LinkMenu.qml
        qml/PostDelegate/LinkPreview.qml
        qml/PostDelegate/MediaContainer.qml
        qml/PostDelegate/PostContent.qml
        qml/PostDelegate/PostInfoBar.qml
        qml/PostDelegate/PostDelegate.qml
        qml/PostDelegate/PostLayout.qml
        qml/PostDelegate/PostMenu.qml
        qml/PostDelegate/PostPoll.qml
        qml/PostDelegate/PostTags.qml
        qml/PostDelegate/PostInteractionLabel.qml
        qml/PostDelegate/VideoAttachment.qml
        qml/PostDelegate/QuotePost.qml
        qml/PostDelegate/AudioAttachment.qml
        qml/PostDelegate/FilterNotice.qml
        qml/PostDelegate/ContentNotice.qml

        # Other
        qml/AccountHeader.qml
        qml/AccountInfo.qml
        qml/AnnouncementsPage.qml
        qml/ConversationDelegate.qml
        qml/ConversationPage.qml
        qml/EditListPage.qml
        qml/ExplorePage.qml
        qml/FollowingPage.qml
        qml/FullScreenImage.qml
        qml/LanguageSelector.qml
        qml/ListsPage.qml
        qml/Main.qml
        qml/ListTimelinePage.qml
        qml/MastoPage.qml
        qml/NotificationPage.qml
        qml/ReadMarker.qml
        qml/ReportDialog.qml
        qml/SearchField.qml
        qml/SearchPage.qml
        qml/SearchView.qml
        qml/ShareDialog.qml
        qml/SocialGraphPage.qml
        qml/StandaloneComposer.qml
        qml/ThreadPage.qml
        qml/TimelinePage.qml
        qml/UserInfo.qml
        qml/ServerInformationPage.qml
        qml/GenericServerInformationPage.qml
        qml/AccountSwitchDialog.qml
)

set_source_files_properties(../org.kde.tokodon.svg PROPERTIES
    QT_RESOURCE_ALIAS /org.kde.tokodon.svg
)

qt_target_qml_sources(tokodon_static
    PREFIX /
    RESOURCES
        content/elephant.svg
        content/mastodon-logo.svg
        ../org.kde.tokodon.svg
)

kconfig_target_kcfg_file(tokodon_static FILE config.kcfg CLASS_NAME Config MUTATORS GENERATE_PROPERTIES GENERATE_MOC DEFAULT_VALUE_GETTERS PARENT_IN_CONSTRUCTOR SINGLETON QML_REGISTRATION)
kconfig_target_kcfg_file(tokodon_static FILE accountconfig.kcfg CLASS_NAME AccountConfig MUTATORS GENERATE_PROPERTIES GENERATE_MOC DEFAULT_VALUE_GETTERS PARENT_IN_CONSTRUCTOR QML_REGISTRATION QML_UNCREATABLE)

target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/search)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/timeline)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/account)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/notification)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/network)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/admin)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/conversation)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/editor)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/datatypes)

if (BUILD_TESTING)
    qt_target_qml_sources(tokodon_static
            QML_FILES
            qml/Debug/DebugPage.qml
    )
endif()

if(ANDROID)
    target_sources(tokodon_static
        PRIVATE
        utils/androidutils.cpp
        utils/androidutils.h
    )
    qt_target_qml_sources(tokodon_static
        QML_FILES
        qml/Android/ShareAction.qml
    )
    set_source_files_properties(qml/Android/ShareAction.qml PROPERTIES
        QT_RESOURCE_ALIAS qml/ShareAction.qml
    )
else()
    qt_target_qml_sources(tokodon_static
        QML_FILES
        qml/ShareAction.qml
    )
endif()

if (USE_QTWEBVIEW)
    qt_target_qml_sources(tokodon_static
        QML_FILES
        qml/LoginFlow/WebViewAuthorization.qml
    )
    target_link_libraries(tokodon_static PUBLIC Qt::WebView)
    target_compile_definitions(tokodon_static PUBLIC -DHAVE_WEBVIEW)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(tokodon_static PUBLIC -DTOKODON_DEBUG)
endif()

target_link_libraries(tokodon_static
    PUBLIC
        Qt::Quick
        Qt::Qml
        Qt::Gui
        Qt::Network
        Qt::WebSockets
        Qt::QuickControls2
        Qt::Multimedia
        KF6::I18nQml
        KF6::I18n
        KF6::Kirigami
        KF6::KirigamiPlatform
        KF6::Notifications
        KF6::ConfigCore
        KF6::ConfigGui
        KF6::CoreAddons
        KF6::ColorScheme
        KirigamiAddonsStatefulApp
        ${QTKEYCHAIN_LIBRARIES}
        QCoro::Core
        QCoro::Network
)
target_include_directories(tokodon_static PRIVATE ${CMAKE_BINARY_DIR})

if (TARGET KF6::KIOWidgets)
    target_compile_definitions(tokodon_static PUBLIC -DHAVE_KIO)
    target_link_libraries(tokodon_static PUBLIC KF6::KIOWidgets)
endif()

if (TARGET KUnifiedPush)
    target_compile_definitions(tokodon_static PUBLIC -DHAVE_KUNIFIEDPUSH)
    target_link_libraries(tokodon_static PUBLIC KUnifiedPush)
endif()

if (SAILFISHOS)
    target_compile_definitions(tokodon_static PUBLIC -DSAILFISHOS)
endif()

if ("$ENV{KDECI_BUILD}" STREQUAL "TRUE")
    target_compile_definitions(tokodon_static PUBLIC -DKDECI)
endif()

if(TOKODON_FLATPAK)
    target_compile_definitions(tokodon_static PUBLIC TOKODON_FLATPAK)
endif()

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

add_executable(tokodon
    main.cpp
)

target_include_directories(tokodon PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(tokodon PRIVATE tokodon_static tokodon_staticplugin KirigamiApp)

if(ANDROID)
    target_link_libraries(tokodon PRIVATE Qt::Svg OpenSSL::SSL)
    install(FILES ../org.kde.tokodon.svg  DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami/breeze-internal/icons/ RENAME org.kde.tokodon.svg)
    file(READ icon-list.txt TOKODON_ICONS)
    string(REPLACE "\n" ";" TOKODON_ICONS ${TOKODON_ICONS})
    kirigami_package_breeze_icons(ICONS ${TOKODON_ICONS} ${KIRIGAMI_ADDONS_ICONS})
else()
    target_link_libraries(tokodon PRIVATE Qt::Widgets)
endif()

if (TARGET KF6::DBusAddons AND NOT HAIKU)
    target_link_libraries(tokodon_static PRIVATE KF6::WindowSystem)
    target_compile_definitions(tokodon_static PUBLIC -DHAVE_DBUS)
    target_link_libraries(tokodon PRIVATE KF6::DBusAddons KF6::WindowSystem)
    target_compile_definitions(tokodon PRIVATE -DHAVE_KDBUSADDONS)
endif ()

if(ANDROID)
    target_sources(tokodon PRIVATE notifyrc.qrc)

    ecm_add_android_apk(tokodon ANDROID_DIR ${CMAKE_SOURCE_DIR}/android)
else()
    install(FILES tokodon.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})

    if (TARGET KUnifiedPush)
        configure_file(org.kde.tokodon.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.tokodon.service)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.tokodon.service DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})
    endif()
endif()

install(TARGETS tokodon ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if(BUILD_TESTING AND NOT ANDRIOID)
    add_definitions(-DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/autotests/data" )

    add_executable(tokodon-offline
            main.cpp
            autotests/mockaccount.cpp)
    target_compile_definitions(tokodon-offline PUBLIC TEST_MODE=1)
    target_link_libraries(tokodon-offline PRIVATE tokodon_static tokodon_staticplugin Qt::Widgets KirigamiApp)
    target_include_directories(tokodon-offline PRIVATE ${CMAKE_BINARY_DIR})

    if (TARGET KF6::DBusAddons)
        target_link_libraries(tokodon-offline PRIVATE KF6::DBusAddons KF6::WindowSystem)
        target_compile_definitions(tokodon-offline PRIVATE -DHAVE_KDBUSADDONS)
    endif ()
endif()

if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT TOKODON_FLATPAK)
    add_subdirectory(purpose)
endif ()
