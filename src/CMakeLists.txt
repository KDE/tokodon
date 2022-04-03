add_executable(tokodon
    accountmanager.cpp
    account.cpp
    post.cpp
    main.cpp
    threadmodel.cpp
    accountmodel.cpp
    timelinemodel.cpp
    attachmenteditormodel.cpp
    clipboard.cpp
    relationship.cpp
    resources.qrc
)

target_include_directories(tokodon PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(tokodon PRIVATE Qt::Quick Qt::Qml Qt::Gui Qt::Network Qt::WebSockets Qt::QuickControls2 KF5::I18n KF5::Kirigami2 KF5::Notifications KF5::ConfigCore KF5::ConfigGui KF5::CoreAddons ${QTKEYCHAIN_LIBRARIES})
kconfig_add_kcfg_files(tokodon GENERATE_MOC config.kcfgc)

if(ANDROID)
    target_link_libraries(tokodon PRIVATE Qt5::Svg OpenSSL::SSL)
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
    )
else()
    target_link_libraries(tokodon PRIVATE Qt5::Widgets)
endif()

if(TARGET KF5::DBusAddons)
    target_link_libraries(tokodon PRIVATE KF5::DBusAddons)
    target_compile_definitions(tokodon PRIVATE -DHAVE_KDBUSADDONS)
endif()

install(TARGETS tokodon ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
