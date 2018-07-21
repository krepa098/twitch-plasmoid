cmake_minimum_required(VERSION 3.0)

project(plasma-twitch)
 
find_package(ECM 1.4.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(ECMInstallIcons)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(KF5 REQUIRED COMPONENTS
    Plasma
    Notifications
    I18n
)

find_package(Qt5 CONFIG REQUIRED COMPONENTS
    Quick
    Core
    Network
    Qml
)

add_subdirectory(plugin)
add_subdirectory(icons)
plasma_install_package(plasmoid org.kde.plasma.twitch)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
