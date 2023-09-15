# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "23")
set(RELEASE_SERVICE_VERSION_MINOR "11")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(tokodon VERSION ${RELEASE_SERVICE_VERSION})

option(SAILFISHOS "Build for sailfishOS" OFF)

set(KF_MIN_VERSION "5.77.0")
set(QT_MIN_VERSION "5.15.0")
if (ANDROID)
    set(QT_MIN_VERSION "5.15.10")
endif()

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(ECMQMLModules)
include(KDEClangFormat)
include(ECMQtDeclareLoggingCategory)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMAddTests)
include(KDEGitCommitHooks)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX TOKODON
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/tokodon-version.h
)

find_package(Qt6 ${QT_MIN_VERSION} NO_MODULE COMPONENTS Widgets Core Quick Gui QuickControls2 Svg WebSockets Test)
set_package_properties(Qt6 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Basic application components"
)

find_package(KF6 ${KF_MIN_VERSION} COMPONENTS Kirigami2 I18n Notifications Config CoreAddons ColorScheme)
set_package_properties(KF6 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Basic application components"
)

find_package(KF6KirigamiAddons 0.10 REQUIRED)
set_package_properties(KF6KirigamiAddons PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required runtime dependency (all module are required)"
)

find_package(Qt6Keychain)
set_package_properties(Qt6Keychain PROPERTIES
    TYPE REQUIRED
    PURPOSE "Secure storage of account secrets"
)

if (ANDROID)
    find_package(OpenSSL)
    set_package_properties(OpenSSL PROPERTIES
        TYPE REQUIRED
        PURPOSE "Encrypted communications"
    )
else()
    if (NOT SAILFISHOS)
        find_package(KF6QQC2DesktopStyle REQUIRED)
    endif()
endif ()

if (UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS KIO)
endif ()

if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT TOKODON_FLATPAK)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Purpose)
endif ()

if (NOT ANDROID AND NOT WIN32 AND NOT APPLE)
    find_package(KF6DBusAddons ${KF_MIN_VERSION} REQUIRED)
    find_package(KF6WindowSystem ${KF_MIN_VERSION} REQUIRED)
endif ()

find_package(PkgConfig REQUIRED)
pkg_check_modules(MPV REQUIRED mpv)

ecm_find_qmlmodule(org.kde.kitemmodels 1.0)
ecm_find_qmlmodule(org.kde.sonnet 1.0)

add_definitions(-DQT_NO_FOREACH -DQT_NO_KEYWORDS)

add_subdirectory(src)

ki18n_install(po)

if (ANDROID)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/android/version.gradle.in ${CMAKE_BINARY_DIR}/version.gradle)
    kirigami_package_breeze_icons(ICONS bookmarks)
endif()

install(FILES org.kde.tokodon.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.tokodon.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.tokodon.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

ecm_qt_install_logging_categories(
    EXPORT TOKODON
    FILE tokodon.categories
    SORT DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
