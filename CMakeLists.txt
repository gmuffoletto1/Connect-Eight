cmake_minimum_required(VERSION 3.16)

project(ConnectEight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_library(connect_eight_lib
    src/AI.cpp
    src/Board.cpp
    src/GUI.cpp
    src/Game.cpp
)

target_include_directories(connect_eight_lib PUBLIC include)
target_link_libraries(connect_eight_lib PUBLIC sfml-graphics sfml-window sfml-system)

add_executable(connect_eight
    src/main.cpp
)

target_link_libraries(connect_eight PRIVATE connect_eight_lib)

enable_testing()

add_executable(test_board tests/test_board.cpp)
target_link_libraries(test_board PRIVATE connect_eight_lib)
add_test(NAME board COMMAND test_board)

add_executable(test_game tests/test_game.cpp)
target_link_libraries(test_game PRIVATE connect_eight_lib)
add_test(NAME game COMMAND test_game)

add_executable(test_ai tests/test_ai.cpp)
target_link_libraries(test_ai PRIVATE connect_eight_lib)
add_test(NAME ai COMMAND test_ai)

add_executable(test_gui tests/test_gui.cpp)
target_link_libraries(test_gui PRIVATE connect_eight_lib)

find_program(XVFB_RUN xvfb-run)
if(XVFB_RUN)
    add_test(NAME gui COMMAND ${XVFB_RUN} -a $<TARGET_FILE:test_gui>)
else()
    add_test(NAME gui COMMAND test_gui)
endif()
