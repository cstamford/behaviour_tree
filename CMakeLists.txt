cmake_minimum_required(VERSION 3.23)
project(behaviour_tree VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SRC src/*.cpp src/*.hpp)
add_library(behaviour_tree ${SRC})
target_include_directories(behaviour_tree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE TEST_SRC test/*.cpp test/*.hpp)
add_executable(bt_tests ${TEST_SRC})
target_link_libraries(bt_tests behaviour_tree)
