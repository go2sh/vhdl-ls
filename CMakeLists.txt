cmake_minimum_required (VERSION 3.9)
project(vhdl-ls)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/../json/src)
include_directories(${CMAKE_SOURCE_DIR}/../vc/include)
include_directories(${CMAKE_SOURCE_DIR}/../liblsp/include)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(src/)

link_directories(${CMAKE_SOURCE_DIR}/../vc/build/lib)
link_directories(${CMAKE_SOURCE_DIR}/../liblsp/build/lib)

add_subdirectory(src)
