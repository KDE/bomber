ecm_setup_version(${bomber_VERSION}
    VARIABLE_PREFIX BOMBER
    VERSION_HEADER bomber_version.h
)

add_executable(bomber)

target_sources(bomber PRIVATE
    board.cpp
    board.h
    bomb.cpp
    bomber.cpp
    bomber.h
    bomberwidget.cpp
    bomberwidget.h
    building.cpp
    bomb.h
    explodable.cpp
    explodable.h
    main.cpp
    plane.cpp
    plane.h

    bomber.qrc
)

kconfig_add_kcfg_files(bomber settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/*-apps-bomber.png")
ecm_add_app_icon(bomber ICONS ${ICONS_SRCS})

target_link_libraries(bomber
    KF5KDEGames
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::DBusAddons
    KF${KF_MAJOR_VERSION}::Crash
)

install(TARGETS bomber ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.bomber.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.bomber.appdata.xml DESTINATION  ${KDE_INSTALL_METAINFODIR})
install(FILES bomber.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR})
