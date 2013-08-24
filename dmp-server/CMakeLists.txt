cmake_minimum_required(VERSION 2.8)

add_definitions("--std=c++11")

add_executable(dmp-server main.cpp RTSPReceiver.hpp RTSPReceiver.cpp)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-library/include)
target_link_libraries(dmp-server dmp-library)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-common)
target_link_libraries(dmp-server dmp-common)

find_package(LiveMedia REQUIRED)
include_directories(${LIVEMEDIA_INCLUDE_DIRS})
target_link_libraries(dmp-server ${LIVEMEDIA_LIBRARIES})

find_package(Boost COMPONENTS system serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-server ${Boost_LIBRARIES})
