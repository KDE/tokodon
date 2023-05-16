# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_library(tokodon_static STATIC)

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
    # Account
    account/abstractaccount.cpp
    account/abstractaccount.h
    account/accountmanager.cpp
    account/accountmanager.h
    account/account.cpp
    account/account.h
    account/preferences.cpp
    account/preferences.h
    account/identity.cpp
    account/identity.h
    account/socialgraphmodel.cpp
    account/socialgraphmodel.h
    account/relationship.cpp
    account/profileeditor.cpp
    account/notificationhandler.cpp
    account/notificationhandler.h

    # Editor
    editor/posteditorbackend.cpp
    editor/posteditorbackend.h
    editor/attachmenteditormodel.cpp
    editor/attachmenteditormodel.h
    editor/polltimemodel.cpp
    editor/polltimemodel.h
    editor/languagemodel.cpp
    editor/languagemodel.h
    editor/polleditorbackend.cpp
    editor/polleditorbackend.h

    # Conversation view
    conversation/conversationmodel.cpp
    conversation/conversationmodel.h

    # Timeline
    timeline/post.cpp
    timeline/post.h
    timeline/poll.cpp
    timeline/poll.h
    timeline/accountmodel.cpp
    timeline/accountmodel.h
    timeline/threadmodel.cpp
    timeline/threadmodel.h
    timeline/timelinemodel.cpp
    timeline/timelinemodel.h
    timeline/notificationmodel.cpp
    timeline/notificationmodel.h
    timeline/tagsmodel.cpp
    timeline/tagsmodel.h
    timeline/maintimelinemodel.cpp
    timeline/maintimelinemodel.h
    timeline/notificationmodel.cpp
    timeline/notificationmodel.h
    timeline/abstracttimelinemodel.cpp
    timeline/abstracttimelinemodel.h

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
    utils/clipboard.cpp
    utils/clipboard.h
    utils/messagefiltercontainer.cpp
    utils/messagefiltercontainer.h
    utils/mpvplayer.cpp
    utils/mpvplayer.h
    utils/qthelper.hpp
    utils/utils.cpp
    utils/utils.h

    # Network related classes
    network/networkrequestprogress.cpp
    network/networkrequestprogress.h
    network/networkaccessmanagerfactory.cpp
    network/networkaccessmanagerfactory.h
    network/networkcontroller.cpp
    network/networkcontroller.h

    # Admin
    admin/accounttoolmodel.cpp
    admin/adminaccountinfo.cpp
)

if(NOT ANDROID)
    target_sources(tokodon_static
    PRIVATE
        utils/colorschemer.cpp
        utils/colorschemer.h
    )
endif()

kconfig_add_kcfg_files(tokodon_static GENERATE_MOC config.kcfgc)
target_link_libraries(tokodon_static PUBLIC Qt::Quick Qt::Qml Qt::Gui Qt::Network Qt::WebSockets Qt::QuickControls2 KF${QT_MAJOR_VERSION}::I18n KF${QT_MAJOR_VERSION}::Kirigami2 KF${QT_MAJOR_VERSION}::Notifications KF${QT_MAJOR_VERSION}::ConfigCore KF${QT_MAJOR_VERSION}::ConfigGui KF${QT_MAJOR_VERSION}::CoreAddons ${QTKEYCHAIN_LIBRARIES} ${MPV_LIBRARIES})
target_include_directories(tokodon_static PUBLIC ${MPV_INCLUDE_DIRS})
target_compile_options(tokodon_static PUBLIC ${MPV_CFLAGS_OTHER} -fexceptions)
target_link_directories(tokodon_static PUBLIC ${MPV_LIBRARY_DIRS})

if (TARGET KF${QT_MAJOR_VERSION}::KIOWidgets)
    target_compile_definitions(tokodon_static PUBLIC -DHAVE_KIO)
    target_link_libraries(tokodon_static PUBLIC KF${QT_MAJOR_VERSION}::KIOWidgets)
endif ()

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
        "favourite"
        "cell_edit"
        "folder-chart"
    )
else()
    target_link_libraries(tokodon PRIVATE Qt::Widgets)
    target_compile_definitions(tokodon PRIVATE -DHAVE_COLORSCHEME)
    target_link_libraries(tokodon_static PUBLIC KF${QT_MAJOR_VERSION}::ConfigWidgets)
endif()

if (TARGET KF${QT_MAJOR_VERSION}::DBusAddons)
    target_link_libraries(tokodon PRIVATE KF${QT_MAJOR_VERSION}::DBusAddons KF${QT_MAJOR_VERSION}::WindowSystem)
    target_compile_definitions(tokodon PRIVATE -DHAVE_KDBUSADDONS)
endif ()

if(ANDROID)
    target_sources(tokodon PRIVATE notifyrc.qrc)
else()
    install(FILES tokodon.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
endif()

install(TARGETS tokodon ${KF${QT_MAJOR_VERSION}_INSTALL_TARGETS_DEFAULT_ARGS})
