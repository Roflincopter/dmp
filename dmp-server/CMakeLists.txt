cmake_minimum_required(VERSION 2.8)

add_definitions("--std=c++11")

add_executable(dmp-server main.cpp RTSPReceiver.hpp RTSPReceiver.cpp)

find_package(LiveMedia REQUIRED)
include_directories(${LIVEMEDIA_INCLUDE_DIRS})
target_link_libraries(dmp-server ${LIVEMEDIA_LIBRARIES})
