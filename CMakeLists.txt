cmake_minimum_required (VERSION 3.9)
project(vhdl-ls)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(src/)

add_subdirectory(src)
