cmake_minimum_required(VERSION 2.8)

add_definitions("--std=c++11")

add_executable(dmp-client   main.cpp
                            dmp_client.hpp dmp_client.cpp
              )

if(MINGW)
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_STATIC_RUNTIME ON)
endif(MINGW)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-library/include)
target_link_libraries(dmp-client dmp-library)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-common)
target_link_libraries(dmp-client dmp-common)

find_package(Boost COMPONENTS system serialization program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-client ${Boost_LIBRARIES})

find_package(GStreamer REQUIRED)
include_directories(${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(dmp-server ${GTREAMER_LIBRARIES})

if(MINGW)
	add_definitions("-DBOOST_THREAD_USE_LIB=1")
	target_link_libraries(dmp-client ws2_32 mswsock)
endif(MINGW)
