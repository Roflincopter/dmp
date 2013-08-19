cmake_minimum_required(VERSION 2.8)

add_definitions("--std=c++11")

add_executable(dmp-client main.cpp RTSPSender.hpp RTSPSender.cpp)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

find_package(LiveMedia REQUIRED)
include_directories(${LIVEMEDIA_INCLUDE_DIRS})
target_link_libraries(dmp-client ${LIVEMEDIA_LIBRARIES})

find_package(LibDMP-library REQUIRED)
include_directories(${LIBDMP-LIBRARY_INCLUDE_DIRS})
target_link_libraries(dmp-client ${LIBDMP-LIBRARY_LIBRARIES})

find_package(Boost COMPONENTS system serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-client ${Boost_LIBRARIES})
