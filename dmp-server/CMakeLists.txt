cmake_minimum_required(VERSION 2.8)

add_definitions("--std=c++11")

add_executable(dmp-server main.cpp
						  dmp_server.hpp dmp_server.cpp
						  client_endpoint.hpp client_endpoint.cpp
			  )

include_directories(${CMAKE_SOURCE_DIR}/libdmp-library/include)
target_link_libraries(dmp-server dmp-library)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-common)
target_link_libraries(dmp-server dmp-common)

find_package(Boost COMPONENTS thread system serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-server ${Boost_LIBRARIES})

find_package(GLIB2 REQUIRED)
include_directories(${GLIB2_INCLUDE_DIRS})

find_package(GStreamer REQUIRED)
include_directories(${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(dmp-server ${GTREAMER_LIBRARIES})

if(MINGW)
	add_definitions("-DBOOST_THREAD_USE_LIB=1")
	target_link_libraries(dmp-server ws2_32 mswsock)
endif(MINGW)
