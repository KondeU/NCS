cmake_minimum_required(VERSION 3.20)
project(test)

file(GLOB_RECURSE SRC *.cpp)
add_executable(${PROJECT_NAME} ${SRC} ${INC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/.. FILES ${SRC} ${INC})
