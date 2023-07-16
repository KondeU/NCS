cmake_minimum_required(VERSION 3.20)
project(sample)

file(GLOB_RECURSE SRC *.hpp *.cpp)
add_executable(${PROJECT_NAME} ${SRC} ${INC})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/.. FILES ${SRC} ${INC})
