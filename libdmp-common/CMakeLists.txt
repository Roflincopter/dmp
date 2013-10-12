CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

add_library(dmp-common SHARED connection.hpp connection.cpp accept.hpp accept.cpp connect.hpp connect.cpp message.hpp message_serializer.hpp)

find_package(Boost COMPONENTS system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-common ${Boost_LIBRARIES})


