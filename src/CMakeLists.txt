ecm_setup_version(${bomber_VERSION}
    VARIABLE_PREFIX BOMBER
    VERSION_HEADER bomber_version.h
)

add_executable(bomber)

target_sources(bomber PRIVATE
    bomber.cpp
    main.cpp
    bomberwidget.cpp
    board.cpp
    plane.cpp
    bomb.cpp
    building.cpp
    explodable.cpp

    bomber.qrc
)

kconfig_add_kcfg_files(bomber settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/*-apps-bomber.png")
ecm_add_app_icon(bomber ICONS ${ICONS_SRCS})

target_link_libraries(bomber
    KF5KDEGames
    KF5::XmlGui
    KF5::I18n
    KF5::DBusAddons
    KF5::Crash
)

install(TARGETS bomber ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.bomber.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.bomber.appdata.xml DESTINATION  ${KDE_INSTALL_METAINFODIR})
install(FILES bomber.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR})
