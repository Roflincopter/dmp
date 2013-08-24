cmake_minimum_required(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

add_definitions(--std=c++11)

add_subdirectory(libdmp-library)
add_subdirectory(libdmp-common)

add_subdirectory(dmp-server)
add_subdirectory(dmp-client)
