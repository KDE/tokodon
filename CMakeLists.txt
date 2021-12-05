cmake_minimum_required(VERSION 3.1)

project(tokodon)
set(PROJECT_VERSION "21.12")

set(KF5_MIN_VERSION "5.77.0")
set(QT_MIN_VERSION "5.15.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(ECMQMLModules)
include(KDEClangFormat)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX TOKODON
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/tokodon-version.h
)

find_package(Qt5 ${QT_MIN_VERSION} NO_MODULE COMPONENTS Widgets Core Quick Gui QuickControls2 Multimedia Svg WebSockets)
set_package_properties(Qt5 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Basic application components"
)
find_package(KF5 ${KF5_MIN_VERSION} COMPONENTS Kirigami2 I18n Notifications Config CoreAddons)
set_package_properties(KF5 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Basic application components"
)
set_package_properties(KF5Kirigami2 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Kirigami application UI framework"
)

if(ANDROID)
    find_package(OpenSSL)
    set_package_properties(OpenSSL PROPERTIES
        TYPE REQUIRED
        PURPOSE "Encrypted communications"
    )
else()
    find_package(KF5QQC2DesktopStyle REQUIRED)
    find_package(Qt5Keychain)
    set_package_properties(Qt5Keychain PROPERTIES
        TYPE REQUIRED
        PURPOSE "Secure storage of account secrets"
    )
endif()

if (NOT ANDROID AND NOT WIN32 AND NOT APPLE)
    find_package(KF5DBusAddons ${KF5_MIN_VERSION} REQUIRED)
endif()

ecm_find_qmlmodule(org.kde.kitemmodels 1.0)

add_definitions(-DQT_NO_FOREACH -DQT_NO_KEYWORDS)

add_subdirectory(src)

install(FILES org.kde.tokodon.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.tokodon.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.tokodon.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)


feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
