cmake_minimum_required(VERSION 3.7)

project(MathBeauty)

set(CMAKE_CXX_STANDART 11)

find_package(SDL2 REQUIRED)
include_directories(MathBeauty ${SDL2_INCLUDE_DIRS})

set(CMAKE_EXECUTABLE_SUFFIX ".out")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "-mavx2 -D NDEBUG -Wall -Wextra -pedantic -O3")

add_subdirectory(src)