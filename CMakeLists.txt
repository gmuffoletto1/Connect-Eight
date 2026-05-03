cmake_minimum_required(VERSION 3.16)

project(ConnectEight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(connect_eight
    src/AI.cpp
    src/Board.cpp
    src/GUI.cpp
    src/Game.cpp
    src/main.cpp
)

target_include_directories(connect_eight PRIVATE include)
target_link_libraries(connect_eight PRIVATE sfml-graphics sfml-window sfml-system)
