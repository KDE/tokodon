# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

set(debug_files_SRCS)

ecm_qt_declare_logging_category(debug_files_SRCS
    HEADER tokodon_debug.h
    IDENTIFIER TOKODON_LOG
    CATEGORY_NAME org.kde.tokodon
    DESCRIPTION "tokodon"
    DEFAULT_SEVERITY Warning
    EXPORT TOKODON
)

ecm_qt_declare_logging_category(debug_files_SRCS
    HEADER tokodon_http_debug.h
    IDENTIFIER TOKODON_HTTP
    CATEGORY_NAME org.kde.tokodon.http
    DESCRIPTION "Tokodon request"
    EXPORT TOKODON
)

set(tokodon_src
    accountmanager.cpp
    account.cpp
    post.cpp
    posteditorbackend.cpp
    poll.cpp
    utils.cpp
    preferences.cpp
    filetransferjob.cpp
    filehelper.cpp
    threadmodel.cpp
    accountmodel.cpp
    timelinemodel.cpp
    attachmenteditormodel.cpp
    networkrequestprogress.cpp
    notificationmodel.cpp
    clipboard.cpp
    relationship.cpp
    networkaccessmanagerfactory.cpp
    notificationhandler.cpp
    notificationmodel.cpp
    abstracttimelinemodel.cpp
    abstractaccount.cpp
    messagefiltercontainer.cpp
    identity.cpp
    blurhash.cpp
    blurhashimageprovider.cpp
    networkcontroller.cpp
    searchmodel.cpp
    conversationmodel.cpp
    tagsmodel.cpp
    maintimelinemodel.cpp
    profileeditor.cpp
    linkpaginatedtimelinemodel.cpp
    ${debug_files_SRCS}
)

if(NOT ANDROID)
    set(tokodon_src ${tokodon_src}
            colorschemer.cpp)
endif()

add_library(tokodon_static STATIC ${tokodon_src})
kconfig_add_kcfg_files(tokodon_static GENERATE_MOC config.kcfgc)
target_link_libraries(tokodon_static PUBLIC Qt::Quick Qt::Qml Qt::Gui Qt::Network Qt::WebSockets Qt::QuickControls2 KF5::I18n KF5::Kirigami2 KF5::Notifications KF5::ConfigCore KF5::ConfigGui KF5::CoreAddons ${QTKEYCHAIN_LIBRARIES})

if (TARGET KF5::KIOWidgets)
    target_compile_definitions(tokodon_static PUBLIC -DHAVE_KIO)
    target_link_libraries(tokodon_static PUBLIC KF5::KIOWidgets)
endif()

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

add_executable(tokodon
    main.cpp
    resources.qrc
)

target_include_directories(tokodon PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(tokodon PRIVATE tokodon_static)

if(ANDROID)
    target_link_libraries(tokodon PRIVATE Qt::Svg OpenSSL::SSL)
    kirigami_package_breeze_icons(ICONS
        "help-about"
        "im-user"
        "im-invisible-user"
        "im-kick-user"
        "mail-attachment"
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
        "system-users"
        "go-home-large"
        "view-hidden"
    )
else()
    target_link_libraries(tokodon PRIVATE Qt::Widgets)
    target_compile_definitions(tokodon PRIVATE -DHAVE_COLORSCHEME)
    target_link_libraries(tokodon_static PUBLIC KF5::ConfigWidgets)
endif()

if(TARGET KF5::DBusAddons)
    target_link_libraries(tokodon PRIVATE KF5::DBusAddons KF5::WindowSystem)
    target_compile_definitions(tokodon PRIVATE -DHAVE_KDBUSADDONS)
endif()

if(ANDROID)
    target_sources(tokodon PRIVATE notifyrc.qrc)
else()
    install(FILES tokodon.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
endif()

install(TARGETS tokodon ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
