# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "08")
set(RELEASE_SERVICE_VERSION_MICRO "3")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(tokodon VERSION ${RELEASE_SERVICE_VERSION})

option(SAILFISHOS "Build for sailfishOS" OFF)

set(KF_MIN_VERSION "6.12.0")
set(QT_MIN_VERSION "6.8.0")

include(FeatureSummary)

find_package(ECM ${KF_MIN_VERSION} NO_MODULE)
set_package_properties(ECM PROPERTIES
    TYPE REQUIRED
    URL "https://api.kde.org/ecm/"
    DESCRIPTION "extra cmake modules")

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 ${QT_MIN_VERSION} NO_MODULE COMPONENTS Widgets Core Quick Gui QuickControls2 Svg WebSockets Test Multimedia)
set_package_properties(Qt6 PROPERTIES
        TYPE REQUIRED
        PURPOSE "Required application components"
        )

find_package(Qt6WebView ${QT_MIN_VERSION})
set_package_properties(Qt6WebView PROPERTIES
        TYPE OPTIONAL
        PURPOSE "For in-app authentication on mobile"
)
set(USE_QTWEBVIEW ${Qt6WebView_FOUND})

find_package(KF6 ${KF_MIN_VERSION} COMPONENTS Kirigami KirigamiPlatform I18n Notifications Config CoreAddons ColorScheme IconThemes)
set_package_properties(KF6 PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components"
)

find_package(KF6Crash ${KF_MIN_VERSION})
set_package_properties(KF6Crash PROPERTIES
        TYPE OPTIONAL
        PURPOSE "For crash handling"
)

find_package(KF6KirigamiAddons 1.1.0)
set_package_properties(KF6KirigamiAddons PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required application components and runtime dependency"
)

find_package(Qt6Keychain)
set_package_properties(Qt6Keychain PROPERTIES
    TYPE REQUIRED
    PURPOSE "Secure storage of account secrets"
)

find_package(KUnifiedPush QUIET)
set_package_properties(KUnifiedPush PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Push notification support"
    URL "https://invent.kde.org/libraries/kunifiedpush"
)

if (TARGET KUnifiedPush)
    find_package(OpenSSL)
    set_package_properties(OpenSSL PROPERTIES
            TYPE REQUIRED
            PURPOSE "Generate push notification keys"
    )
endif()

if (ANDROID AND NOT TARGET OpenSSL::SSL)
    find_package(OpenSSL)
    set_package_properties(OpenSSL PROPERTIES
        TYPE REQUIRED
        PURPOSE "Encrypted communications"
    )
endif()

if (SAILFISHOS OR ANDROID)
    find_package(QQC2BreezeStyle)
    set_package_properties(QQC2BreezeStyle PROPERTIES
            PURPOSE "Preferred Qt Quick Controls style"
    )
else()
    find_package(KF6QQC2DesktopStyle)
    set_package_properties(KF6QQC2DesktopStyle PROPERTIES
            TYPE REQUIRED
            PURPOSE "Preferred Qt Quick Controls style"
    )
endif()

if (UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(KF6KIO ${KF_MIN_VERSION})
    set_package_properties(KF6KIO PROPERTIES
            TYPE REQUIRED
            PURPOSE "Sharing content and downloading files"
    )
endif ()

if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT TOKODON_FLATPAK)
    find_package(KF6Purpose ${KF_MIN_VERSION})
    set_package_properties(KF6Purpose PROPERTIES
            TYPE REQUIRED
            PURPOSE "Sharing content from other KDE applications"
    )
endif ()

if (NOT ANDROID AND NOT WIN32 AND NOT APPLE AND NOT HAIKU)
    find_package(KF6DBusAddons ${KF_MIN_VERSION})
    set_package_properties(KF6DBusAddons PROPERTIES
            TYPE REQUIRED
            PURPOSE "Single application support"
    )
    find_package(KF6WindowSystem ${KF_MIN_VERSION})
    set_package_properties(KF6WindowSystem PROPERTIES
            TYPE REQUIRED
            PURPOSE "Misc windowing operations"
    )
endif ()

find_package(QCoro6 REQUIRED COMPONENTS Core Network)
qcoro_enable_coroutines()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

include(ECMSetupVersion)
include(KDEInstallDirs)
include(ECMFindQmlModule)
include(KDEClangFormat)
include(ECMQtDeclareLoggingCategory)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(KDEGitCommitHooks)
include(ECMQmlModule)
include(ECMDeprecationSettings)
include(ECMCheckOutboundLicense)

if (ANDROID)
    include(ECMAddAndroidApk)
endif ()

qt_policy(SET QTP0004 NEW)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX TOKODON
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/tokodon-version.h
)

ecm_set_disabled_deprecation_versions(
        QT ${QT_MIN_VERSION}
        KF ${KF_MIN_VERSION}
)

ecm_find_qmlmodule(org.kde.kitemmodels REQUIRED)
ecm_find_qmlmodule(org.kde.notification REQUIRED)
ecm_find_qmlmodule(org.kde.kquickcontrolsaddons REQUIRED)
ecm_find_qmlmodule(org.kde.prison REQUIRED)

if (NOT ANDROID)
    ecm_find_qmlmodule(org.kde.sonnet REQUIRED)
endif()

add_subdirectory(src)

ki18n_install(po)

install(FILES org.kde.tokodon.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.tokodon.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.tokodon.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

ecm_qt_install_logging_categories(
    EXPORT TOKODON
    FILE tokodon.categories
    SORT DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.qml)
# CI installs dependency headers to _install and _build, which break the reuse check
# Fixes the test by excluding this directory
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX [[.*(install|build).*/.*]])
ecm_check_outbound_license(LICENSES GPL-3.0-only FILES ${ALL_SOURCE_FILES})
