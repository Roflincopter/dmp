cmake_minimum_required(VERSION 2.8)

add_definitions("--std=c++11")

add_executable(dmp-server main.cpp RTSPReceiver.hpp RTSPReceiver.cpp)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

find_package(LiveMedia REQUIRED)
include_directories(${LIVEMEDIA_INCLUDE_DIRS})
target_link_libraries(dmp-server ${LIVEMEDIA_LIBRARIES})

find_package(LibDMP-library REQUIRED)
include_directories(${LIBDMP-LIBRARY_INCLUDE_DIRS})