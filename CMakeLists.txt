cmake_minimum_required(VERSION 3.20)
project(galacticorbit-screensaver VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)\n\nfind_package(SDL3 REQUIRED CONFIG)
find_package(OpenGL REQUIRED)

add_executable(galacticorbit-screensaver src/main.cpp)
target_link_libraries(galacticorbit-screensaver PRIVATE SDL3::SDL3 OpenGL::GL)\ntarget_compile_definitions(galacticorbit-screensaver PRIVATE GALACTICORBIT_DATA_DIR=\"\${CMAKE_INSTALL_FULL_DATADIR}/galacticorbit\")

if(MSVC)
    target_compile_options(galacticorbit-screensaver PRIVATE /W4)
else()
    target_compile_options(galacticorbit-screensaver PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS galacticorbit-screensaver RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})\ninstall(DIRECTORY assets/planet-textures DESTINATION ${CMAKE_INSTALL_DATADIR}/galacticorbit)\n\noption(GALACTICORBIT_BUILD_PLASMA6 \"Build KDE Plasma 6 wallpaper integration\" OFF)\nif(UNIX AND NOT APPLE AND GALACTICORBIT_BUILD_PLASMA6)\n    add_subdirectory(packaging/plasma6)\nendif()
