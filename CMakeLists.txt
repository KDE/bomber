cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Release Service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "21")
set (RELEASE_SERVICE_VERSION_MINOR "11")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(bomber VERSION ${RELEASE_SERVICE_VERSION})

set (QT_MIN_VERSION "5.15.0")
set (KF5_MIN_VERSION "5.85.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(FeatureSummary)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMSetupVersion)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    Crash
    DBusAddons
    DocTools
    I18n
    XmlGui
)

find_package(KF5KDEGames 7.3.0 REQUIRED)

add_definitions(
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050F00
    -DQT_DEPRECATED_WARNINGS_SINCE=0x060000
    -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055400
    -DKF_DEPRECATED_WARNINGS_SINCE=0x060000
)

add_subdirectory(themes)
add_subdirectory(sounds)
add_subdirectory(icons)
add_subdirectory(src)
add_subdirectory(doc)

ki18n_install(po)
kdoctools_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
