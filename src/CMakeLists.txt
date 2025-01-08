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

    # Conversation view
    conversation/conversationmodel.cpp
    conversation/conversationmodel.h

    # Timeline
    timeline/post.cpp
    timeline/post.h
    timeline/attachment.cpp
    timeline/attachment.h
    timeline/notification.cpp
    timeline/notification.h
    timeline/poll.cpp
    timeline/poll.h
    timeline/tag.h
    timeline/tag.cpp
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
        content/ui/Components/Emoji/EmojiDelegate.qml
        content/ui/Components/Emoji/EmojiGrid.qml
        content/ui/Components/Emoji/EmojiPicker.qml
        content/ui/Components/Emoji/EmojiTonesPicker.qml
        content/ui/Components/ClickableIdentityInfo.qml
        content/ui/Components/FocusedImage.qml
        content/ui/Components/InlineIdentityInfo.qml
        content/ui/Components/PopupShadow.qml
        content/ui/Components/ProfileHeader.qml
        content/ui/Components/RoundedEffect.qml
        content/ui/Components/Sidebar.qml
        content/ui/Components/TimelineView.qml
        content/ui/Components/UserCard.qml
        content/ui/Components/QrCodeMaximizeComponent.qml

        # Initial Setup
        content/ui/InitialSetup/SetupNotifications.qml
        content/ui/InitialSetup/SetupPassword.qml
        content/ui/InitialSetup/SetupWelcome.qml

        # Login Flow
        content/ui/LoginFlow/BaseAuthorization.qml
        content/ui/LoginFlow/BrowserAuthorization.qml
        content/ui/LoginFlow/CodeAuthorization.qml
        content/ui/LoginFlow/LoginIssuePage.qml
        content/ui/LoginFlow/LoginPage.qml
        content/ui/LoginFlow/RegistrationPage.qml
        content/ui/LoginFlow/RulesPage.qml
        content/ui/LoginFlow/ServersPage.qml
        content/ui/LoginFlow/WelcomePage.qml

        # Moderation Tools
        content/ui/ModerationTools/AccountToolPage.qml
        content/ui/ModerationTools/EmailBlockToolPage.qml
        content/ui/ModerationTools/FederationToolPage.qml
        content/ui/ModerationTools/IpRulePage.qml
        content/ui/ModerationTools/MainAccountToolPage.qml
        content/ui/ModerationTools/MainFederationToolPage.qml
        content/ui/ModerationTools/MainIpRulePage.qml
        content/ui/ModerationTools/MainReportToolPage.qml
        content/ui/ModerationTools/ModerationToolsView.qml
        content/ui/ModerationTools/ReportToolPage.qml

        # Notifications
        content/ui/Notifications/AccountWarningDelegate.qml
        content/ui/Notifications/GroupInteractionLabel.qml
        content/ui/Notifications/RelationshipSeveranceDelegate.qml
        content/ui/Notifications/ReportDelegate.qml
        content/ui/Notifications/UserInteractionLabel.qml
        content/ui/Notifications/AnnualReportDelegate.qml

        # Settings
        content/ui/Settings/AccountPage.qml
        content/ui/Settings/AccountsPage.qml
        content/ui/Settings/AppearancePage.qml
        content/ui/Settings/EditProfilePage.qml
        content/ui/Settings/ErrorLogPage.qml
        content/ui/Settings/NetworkProxyPage.qml
        content/ui/Settings/NotificationsPage.qml
        content/ui/Settings/ProfileEditor.qml
        content/ui/Settings/SafetyPage.qml
        content/ui/Settings/TokodonConfigurationView.qml

        # Composer
        content/ui/StatusComposer/EditorAttachmentGrid.qml
        content/ui/StatusComposer/AttachmentInfoDialog.qml
        content/ui/StatusComposer/ComposerPoll.qml
        content/ui/StatusComposer/EmojiDialog.qml
        content/ui/StatusComposer/StatusComposer.qml
        content/ui/StatusComposer/StatusPreview.qml
        content/ui/StatusComposer/ScheduledPostsPage.qml

        # Delegate
        content/ui/PostDelegate/AttachmentGrid.qml
        content/ui/PostDelegate/AttachmentMenu.qml
        content/ui/PostDelegate/InformationBar.qml
        content/ui/PostDelegate/InteractionButton.qml
        content/ui/PostDelegate/LinkMenu.qml
        content/ui/PostDelegate/LinkPreview.qml
        content/ui/PostDelegate/MediaContainer.qml
        content/ui/PostDelegate/PostContent.qml
        content/ui/PostDelegate/PostInfoBar.qml
        content/ui/PostDelegate/PostDelegate.qml
        content/ui/PostDelegate/PostLayout.qml
        content/ui/PostDelegate/PostMenu.qml
        content/ui/PostDelegate/PostPoll.qml
        content/ui/PostDelegate/PostTags.qml
        content/ui/PostDelegate/PostInteractionLabel.qml
        content/ui/PostDelegate/VideoAttachment.qml
        content/ui/PostDelegate/QuotePost.qml
        content/ui/PostDelegate/AudioAttachment.qml

        # Other
        content/ui/AccountHeader.qml
        content/ui/AccountInfo.qml
        content/ui/AnnouncementsPage.qml
        content/ui/ConversationDelegate.qml
        content/ui/ConversationPage.qml
        content/ui/EditListPage.qml
        content/ui/ExplorePage.qml
        content/ui/FollowDelegate.qml
        content/ui/FollowingPage.qml
        content/ui/FullScreenImage.qml
        content/ui/LanguageSelector.qml
        content/ui/ListsPage.qml
        content/ui/Main.qml
        content/ui/ListTimelinePage.qml
        content/ui/MastoPage.qml
        content/ui/NotificationPage.qml
        content/ui/ReadMarker.qml
        content/ui/ReportDialog.qml
        content/ui/SearchField.qml
        content/ui/SearchPage.qml
        content/ui/SearchView.qml
        content/ui/ShareDialog.qml
        content/ui/SocialGraphPage.qml
        content/ui/StandaloneComposer.qml
        content/ui/ThreadPage.qml
        content/ui/TimelinePage.qml
        content/ui/UserInfo.qml
        content/ui/ServerInformationPage.qml
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

target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/search)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/timeline)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/account)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/notification)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/network)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/admin)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/conversation)
target_include_directories(tokodon_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/editor)

if (BUILD_TESTING)
    qt_target_qml_sources(tokodon_static
            QML_FILES
            content/ui/Debug/DebugPage.qml
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
        content/ui/Android/ShareAction.qml
    )
    set_source_files_properties(content/ui/Android/ShareAction.qml PROPERTIES
        QT_RESOURCE_ALIAS content/ui/ShareAction.qml
    )
else()
    qt_target_qml_sources(tokodon_static
        QML_FILES
        content/ui/ShareAction.qml
    )
endif()

if (USE_QTMULTIMEDIA)
    find_package(Qt6Multimedia ${QT_MIN_VERSION} REQUIRED)
    set_package_properties(Qt6Multimedia PROPERTIES
            PURPOSE "Video playback"
            TYPE REQUIRED
    )
    target_link_libraries(tokodon_static PUBLIC Qt::Multimedia)

    qt_target_qml_sources(tokodon_static
            QML_FILES
            content/ui/QtMultimedia/MpvPlayer.qml
    )
else()
    target_sources(tokodon_static
            PRIVATE
            utils/mpvplayer.cpp
            utils/mpvplayer.h
    )
    target_link_libraries(tokodon_static PUBLIC MpvQt::MpvQt)
endif()

if (USE_QTWEBVIEW)
    qt_target_qml_sources(tokodon_static
            QML_FILES
            content/ui/LoginFlow/WebViewAuthorization.qml
    )
    target_link_libraries(tokodon_static PUBLIC Qt::WebView)
    target_compile_definitions(tokodon_static PUBLIC -DHAVE_WEBVIEW)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(tokodon_static PUBLIC -DTOKODON_DEBUG)
endif()

kconfig_add_kcfg_files(tokodon_static GENERATE_MOC config.kcfgc accountconfig.kcfgc)
target_link_libraries(tokodon_static
    PUBLIC
        Qt::Quick
        Qt::Qml
        Qt::Gui
        Qt::Network
        Qt::WebSockets
        Qt::QuickControls2
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
)
target_include_directories(tokodon_static PRIVATE ${CMAKE_BINARY_DIR})

if (TARGET KF6::KIOWidgets)
    target_compile_definitions(tokodon_static PUBLIC -DHAVE_KIO)
    target_link_libraries(tokodon_static PUBLIC KF6::KIOWidgets)
endif()

if (TARGET KUnifiedPush)
    target_sources(tokodon_static PRIVATE
            utils/ecdh.cpp
            utils/ecdh.h)
    target_compile_definitions(tokodon_static PUBLIC -DHAVE_KUNIFIEDPUSH)
    target_link_libraries(tokodon_static PUBLIC KUnifiedPush OpenSSL::Crypto)
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
target_link_libraries(tokodon PRIVATE tokodon_static tokodon_staticplugin)

if (TARGET KF6::Crash)
    target_link_libraries(tokodon PUBLIC KF6::Crash)
endif()

if(ANDROID)
    target_link_libraries(tokodon PRIVATE Qt::Svg OpenSSL::SSL)
    kirigami_package_breeze_icons(ICONS
            "help-about"
            "im-user"
            "im-invisible-user"
            "im-kick-user"
            "mail-attachment"
            "mail-attachment-symbolic"
            "dialog-cancel"
            "preferences-desktop-emoticons"
            "document-open"
            "document-save"
            "document-send"
            "dialog-close"
            "edit-delete-remove"
            "code-context"
            "document-edit"
            "list-user-add"
            "list-add-user"
            "user-others"
            "media-playback-pause"
            "media-playback-start"
            "go-up"
            "go-down"
            "list-add"
            "irc-join-channel"
            "settings-configure"
            "configure"
            "rating"
            "rating-unrated"
            "search"
            "mail-replied-symbolic"
            "edit-copy"
            "gtk-quit"
            "compass"
            "network-connect"
            "notifications"
            "kstars_xplanet"
            "kstars_planets"
            "system-users"
            "go-home-large"
            "view-hidden"
            "favorite"
            "cell_edit"
            "office-chart-bar"
            "im-ban-kick-user"
            "user-group-properties"
            "microphone-sensitivity-muted"
            "cards-block"
            "system-search"
            "bookmarks"
            "checkmark"
            "lock"
            "edit-clear"
            "approved"
            "delete"
            "user-identity"
            "download"
            "edit-reset"
            "dialog-ok"
            "gnumeric-graphguru"
            "mail-message"
            "bookmark-new"
            "window-unpin"
            "pin"
            "edit-entry"
            "edit-cut"
            "system-switch-user"
            "note"
            "view-list-text"
            "view-filter"
            "preferences-desktop-theme-global"
            "preferences-desktop-notification"
            "preferences-system-users"
            "kde"
            "cloud-upload"
            "expand"
            "view-web-browser-dom-tree"
            "window"
            "emblem-shared-symbolic"
            "data-warning"
            "arrow-up"
            "arrow-down"
            "zoom-out"
            "zoom-in"
            "object-rotate-left"
            "object-rotate-right"
            "add-subtitle"
            "unlock"
            "dialog-warning-symbolic"
            "list-remove-user"
            "window-close"
            "window-close-symbolic"
            "view-readermode"
            "view-more-horizontal-symbolic"
            "preferences-security"
            "edit-rename"
            "gtk-add"
            "edit-delete"
            "view-pim-news-symbolic"
            "tag-symbolic"
            "dialog-cancel"
            "view-hidden-symbolic"
            "view-visible-symbolic"
            "data-information"
            "arrow-up-symbolic"
            "arrow-down-symbolic"
            "arrow-left-symbolic"
            "arrow-right-symbolic"
            "network-server-symbolic"
            "globe-symbolic"
            "donate-symbolic"
            "input-keyboard-symbolic"
            "error-symbolic"
            "edit-undo"
            "applications-development-symbolic"
            "internet-services-symbolic"
            "view-conversation-balloon-symbolic"
            "favorite-favorited"
            "boost"
            "boost-boosted"
            "bookmarks-bookmarked"
            "resource-calendar-insert"
            "applications-other-symbolic")
    install(FILES ../org.kde.tokodon.svg  DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami/breeze-internal/icons/ RENAME org.kde.tokodon.svg)
else()
    target_link_libraries(tokodon PRIVATE Qt::Widgets)
endif()

if (TARGET KF6::DBusAddons AND NOT HAIKU)
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

install(TARGETS tokodon ${KF6_INSTALL_TARGETS_DEFAULT_ARGS})

if(BUILD_TESTING AND NOT ANDRIOID)
    add_definitions(-DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/autotests/data" )

    add_executable(tokodon-offline
            main.cpp
            autotests/mockaccount.cpp)
    target_compile_definitions(tokodon-offline PUBLIC TEST_MODE=1)
    target_link_libraries(tokodon-offline PRIVATE tokodon_static tokodon_staticplugin Qt::Widgets)
    target_include_directories(tokodon-offline PRIVATE ${CMAKE_BINARY_DIR})

    if (TARGET KF6::DBusAddons)
        target_link_libraries(tokodon-offline PRIVATE KF6::DBusAddons KF6::WindowSystem)
        target_compile_definitions(tokodon-offline PRIVATE -DHAVE_KDBUSADDONS)
    endif ()

    install(TARGETS tokodon-offline ${KF6_INSTALL_TARGETS_DEFAULT_ARGS})
endif()

if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT TOKODON_FLATPAK)
    add_subdirectory(purpose)
endif ()
