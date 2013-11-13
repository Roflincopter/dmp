cmake_minimum_required(VERSION 2.8)

add_definitions("--std=c++11")

add_executable(dmp-client main.cpp
                          dmp_client.hpp dmp_client.cpp
                          dmp_sender.hpp dmp_sender.cpp
                          dmp_receiver.hpp dmp_receiver.cpp
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
target_link_libraries(dmp-client ${GSTREAMER_LIBRARIES})

find_package(GLIB2 REQUIRED)
include_directories(${GLIB2_INCLUDE_DIRS})
target_link_libraries(dmp-client ${GLIB2_LIBRARIES})

find_package(GObject REQUIRED)
include_directories(${GOBJECT_INCLUDE_DIR})
target_link_libraries(dmp-client ${GOBJECT_LIBRARIES})

if(MINGW)
	add_definitions("-DBOOST_THREAD_USE_LIB=1 -mms-bitfields -DPCRE_STATIC")
	target_link_libraries(dmp-client pthread ws2_32 mswsock ffi)
endif(MINGW)
