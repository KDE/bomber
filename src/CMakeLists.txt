project(bomber)

set(bomber_SRCS
   bomber.cpp
   main.cpp
   bomberwidget.cpp
   board.cpp
   plane.cpp
   bomb.cpp
   building.cpp
   explodable.cpp
 )

kconfig_add_kcfg_files(bomber_SRCS settings.kcfgc )
file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pictures/*-apps-bomber.png")
ecm_add_app_icon(bomber_SRCS ICONS ${ICONS_SRCS})
add_executable(bomber ${bomber_SRCS})

target_link_libraries(bomber
    KF5KDEGames
    Phonon::phonon4qt5
    KF5::XmlGui
    KF5::I18n
    KF5::DBusAddons
    KF5::Crash
)

install(TARGETS bomber ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.bomber.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES org.kde.bomber.appdata.xml DESTINATION  ${CMAKE_INSTALL_METAINFODIR} )
install( FILES bomber.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR} )
install( FILES bomberui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/bomber )
