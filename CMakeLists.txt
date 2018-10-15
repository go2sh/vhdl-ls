cmake_minimum_required (VERSION 3.9)
project(vhdl-ls)

set(CXX_STANDARD_REQUIRED 17)
set(CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if (UNIX)
find_package (Threads REQUIRED)
link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif()

find_package(Asio)

include_directories(${CMAKE_SOURCE_DIR}/../vc/include)
include_directories(${CMAKE_SOURCE_DIR}/../liblsp/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

link_directories(${CMAKE_SOURCE_DIR}/../vc/build/lib)
link_directories(${CMAKE_SOURCE_DIR}/../liblsp/build/lib)

add_subdirectory(src)
