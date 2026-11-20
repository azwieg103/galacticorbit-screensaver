cmake_minimum_required(VERSION 3.20)
project(galacticorbit-screensaver VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
find_package(SDL3 REQUIRED CONFIG)
find_package(OpenGL REQUIRED)

add_executable(galacticorbit-screensaver src/main.cpp)
target_link_libraries(galacticorbit-screensaver PRIVATE SDL3::SDL3 OpenGL::GL)
target_compile_definitions(galacticorbit-screensaver PRIVATE
    GALACTICORBIT_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/galacticorbit"
)

if(MSVC)
    target_compile_options(galacticorbit-screensaver PRIVATE /W4)
else()
    target_compile_options(galacticorbit-screensaver PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS galacticorbit-screensaver RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY assets/planet-textures DESTINATION ${CMAKE_INSTALL_DATADIR}/galacticorbit)

# CPack metadata is shared by the Debian and Ubuntu package jobs.
set(CPACK_PACKAGE_NAME "galacticorbit-screensaver")
set(CPACK_PACKAGE_VENDOR "Zwiegnet")
set(CPACK_PACKAGE_CONTACT "programming@zwiegnet.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Animated Milky Way and Solar System visualization")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/azwieg103/galacticorbit-screensaver")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Galactic Orbit")
set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
include(CPack)

option(GALACTICORBIT_BUILD_PLASMA6 "Build KDE Plasma 6 wallpaper integration" OFF)
if(UNIX AND NOT APPLE AND GALACTICORBIT_BUILD_PLASMA6)
    add_subdirectory(packaging/plasma6)
endif()
