cmake_minimum_required(VERSION 3.20)
project(galacticorbit-plasma6)

find_package(ECM 6.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Plasma REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick ShaderTools)

plasma_install_package(package org.zwiegnet.galacticorbit wallpapers wallpaper)

add_library(galacticorbitgeometryplugin MODULE
    geometry/DisplayGeometry.cpp
    geometry/DisplayGeometryPlugin.cpp
)
target_link_libraries(galacticorbitgeometryplugin PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)
set_target_properties(galacticorbitgeometryplugin PROPERTIES PREFIX "" OUTPUT_NAME "galacticorbitgeometryplugin")

qt_add_shaders(galacticorbitgeometryplugin galacticorbit_shaders
    PREFIX "/org/zwiegnet/galacticorbit/shaders"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    FILES shaders/galaxy.frag
)
install(TARGETS galacticorbitgeometryplugin LIBRARY DESTINATION "${KDE_INSTALL_QMLDIR}/org/zwiegnet/galacticorbit/geometry")
install(FILES geometry/qmldir DESTINATION "${KDE_INSTALL_QMLDIR}/org/zwiegnet/galacticorbit/geometry")
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
