add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.twitch\")

set(twitch_SRCS
    data.cpp
    client.cpp
    channelmodel.cpp
    plugin.cpp
    settings.cpp
)

add_library(twitchplasmoidplugin SHARED ${twitch_SRCS})

target_link_libraries(twitchplasmoidplugin
                      Qt5::Gui
                      Qt5::Qml
                      Qt5::Network
                      KF5::Plasma
                      KF5::Notifications
                      KF5::I18n)

install(TARGETS twitchplasmoidplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/private/twitchplasmoid)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/private/twitchplasmoid)
install(FILES twitchPlasmoid.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})
