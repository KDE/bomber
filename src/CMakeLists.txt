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
#kde4_add_app_icon(bomber_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pictures/hi*-apps-bomber.png")
add_executable(bomber ${bomber_SRCS})

target_link_libraries(bomber KF5KDEGames  Phonon::phonon4qt5 KF5::XmlGui KF5::I18n KF5::DBusAddons)

install(TARGETS bomber ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.bomber.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES bomber.appdata.xml DESTINATION  ${DATA_INSTALL_DIR}/appdata )
install( FILES bomber.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES bomberui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/bomber )

