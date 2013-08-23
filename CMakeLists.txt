cmake_minimum_required(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

add_subdirectory(libdmp-library)

add_subdirectory(dmp-server)
add_subdirectory(dmp-client)
