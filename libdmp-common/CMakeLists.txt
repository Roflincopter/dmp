CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(dmp-common-sources
	connection.hpp connection.cpp
	accept.hpp accept.cpp
	connect.hpp connect.cpp
	message.hpp
	message_outputter.hpp
	message_serializer.hpp
	message_callbacks.hpp
	gstreamer_util.hpp gstreamer_util.cpp
	fusion_static_dispatch.hpp fusion_static_dispatch.cpp
	playlist.hpp
	message_switch.hpp
	number_pool.hpp number_pool.cpp
)

if(MINGW)
	add_library(dmp-common STATIC ${dmp-common-sources})
else(MINGW)
	add_library(dmp-common SHARED ${dmp-common-sources})
endif(MINGW)

include_directories(../libdmp-library/include)

find_package(Boost COMPONENTS system thread REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-common ${Boost_LIBRARIES})

find_package(GStreamer REQUIRED)
include_directories(SYSTEM ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(dmp-common ${GSTREAMER_LIBRARIES})

find_package(GLIB2 REQUIRED)
include_directories(SYSTEM ${GLIB2_INCLUDE_DIRS})
target_link_libraries(dmp-common ${GLIB2_LIBRARIES})

find_package(GObject REQUIRED)
include_directories(SYSTEM ${GOBJECT_INCLUDE_DIR})
target_link_libraries(dmp-common ${GOBJECT_LIBRARIES})

if(USE_GPERF)
	target_link_libraries(dmp-common "-lprofiler")
endif(USE_GPERF)
