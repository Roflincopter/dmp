CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(dmp-common-sources
	message_outputter.hpp
	message_serializer.hpp
	gstreamer_base.hpp gstreamer_base.cpp
	playlist.hpp
	debug_macros.hpp
	time_util.hpp time_util.cpp
	number_pool.hpp number_pool.cpp
	timed_debug.hpp timed_debug.cpp
)

add_library(dmp-common STATIC ${dmp-common-sources})

include_directories(${CMAKE_SOURCE_DIR}/libdmp-library/include/)

find_package(Boost COMPONENTS system serialization REQUIRED)
find_package(GStreamer REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(GObject REQUIRED)

target_link_libraries(dmp-common
	PRIVATE ${GSTREAMER_LIBRARIES}
	PRIVATE ${GSTREAMER_BASE_LIBRARIES}
	PRIVATE ${GLIB2_LIBRARIES}
	PRIVATE ${GOBJECT_LIBRARIES}
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE boost-fusion-utils
)

target_include_directories(dmp-common SYSTEM
	PUBLIC ${GSTREAMER_INCLUDE_DIRS}
	PUBLIC ${GLIB2_INCLUDE_DIRS}
	PRIVATE ${GOBJECT_INCLUDE_DIR}
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_include_directories(dmp-common
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

if(USE_GPERF)
	target_link_libraries(dmp-common "-lprofiler")
endif(USE_GPERF)
