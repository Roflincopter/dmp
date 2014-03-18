cmake_minimum_required(VERSION 2.8)

add_executable(dmp-server main.cpp
                          dmp_server.hpp dmp_server.cpp
                          dmp_radio.hpp dmp_radio.cpp
                          client_endpoint.hpp client_endpoint.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-library/include)
target_link_libraries(dmp-server dmp-library)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-common)
target_link_libraries(dmp-server dmp-common)

find_package(Boost COMPONENTS thread system serialization REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-server ${Boost_LIBRARIES})

find_package(GStreamer REQUIRED)
include_directories(SYSTEM ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(dmp-server ${GSTREAMER_LIBRARIES})

find_package(GLIB2 REQUIRED)
include_directories(SYSTEM ${GLIB2_INCLUDE_DIRS})
target_link_libraries(dmp-server ${GLIB2_LIBRARIES})

find_package(GObject REQUIRED)
include_directories(SYSTEM ${GOBJECT_INCLUDE_DIR})
target_link_libraries(dmp-server ${GOBJECT_LIBRARIES})

if(USE_GPERF)
	target_link_libraries(dmp-server "-lprofiler")
endif(USE_GPERF)

if(MINGW)
	add_definitions("-DBOOST_THREAD_USE_LIB=1 -mms-bitfields -DPCRE_STATIC")
	target_link_libraries(dmp-server pthread ws2_32 mswsock ffi)
endif(MINGW)
