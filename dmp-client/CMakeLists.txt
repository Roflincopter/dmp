cmake_minimum_required(VERSION 2.8)

add_definitions("--std=c++11")

add_executable(dmp-client main.cpp RTSPSender.hpp RTSPSender.cpp)

find_package(LiveMedia REQUIRED)
include_directories(${LIVEMEDIA_INCLUDE_DIRS})
target_link_libraries(dmp-client ${LIVEMEDIA_LIBRARIES})
