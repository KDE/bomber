cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Release Service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "26")
set (RELEASE_SERVICE_VERSION_MINOR "03")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(bomber VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMDeprecationSettings)
include(FeatureSummary)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(KDEClangFormat)
include(KDEGitCommitHooks)

include(InternalMacros)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    Crash
    DocTools
    I18n
    WidgetsAddons
    XmlGui
    IconThemes
)

set(USE_DBUS_DEFAULT OFF)
if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT HAIKU)
    set(USE_DBUS_DEFAULT ON)
endif()
option(USE_DBUS "Build components using DBus (used for interprocess communication, external open in same instance and various presentation related bits)" ${USE_DBUS_DEFAULT})
if(USE_DBUS)
    find_package(KF6DBusAddons ${KF_MIN_VERSION} CONFIG REQUIRED)
    set(HAVE_DBUS 1)
endif()

find_package(KDEGames6 6.0.0 REQUIRED)

ecm_set_disabled_deprecation_versions(
    QT 6.9.0
    KF 6.19.0
    KDEGAMES 6.0
)

add_subdirectory(themes)
add_subdirectory(sounds)
add_subdirectory(icons)
add_subdirectory(src)
add_subdirectory(doc)

ki18n_install(po)
kdoctools_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
