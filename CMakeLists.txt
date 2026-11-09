cmake_minimum_required(VERSION 3.16)
project(galacticorbit-screensaver VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 REQUIRED CONFIG)
find_package(OpenGL REQUIRED)

add_executable(galacticorbit-screensaver src/main.cpp)
target_link_libraries(galacticorbit-screensaver PRIVATE SDL3::SDL3 OpenGL::GL)

if(MSVC)
    target_compile_options(galacticorbit-screensaver PRIVATE /W4)
else()
    target_compile_options(galacticorbit-screensaver PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS galacticorbit-screensaver RUNTIME DESTINATION bin)
