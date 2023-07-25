cmake_minimum_required(VERSION 3.20)
project(NCS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
file(GLOB_RECURSE INC include/*)

add_subdirectory(test)

install(FILES LICENSE.txt DESTINATION .)
install(FILES README.md DESTINATION .)
install(DIRECTORY include/ DESTINATION include)
